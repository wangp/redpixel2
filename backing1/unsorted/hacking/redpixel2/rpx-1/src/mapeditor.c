/* mapedit.c: Red Pixel II el-cheapo map editor 
 *  Most of the work is done by scripts/map*.lua
 */

#include <stdio.h>
#include <lua.h>
#include <tolua.h>
#include <allegro.h>

#include "defs.h"
#include "report.h"
#include "tiles.h"
#include "dirty.h"
#include "mapeditor.h"
#include "rpxfiles.h"


int tolua_allegro_open(void);	       /* allegro.pkg */
int tolua_red_open(void);	       /* red.pkg */


/* exported globals */
BITMAP *editbuf;
int editbuf_w, editbuf_h;

BITMAP *palbuf;
int palbuf_x;
int palbuf_w, palbuf_h;
int palbuf_update;


#if 0				       /* obsolete */

/* 
 * editmode stuff:
 *  Maintain a table of lua-implemented editing 'modes'
 */

typedef struct editmode {
    char *name;
    int switchkey;
    int table_ref;
    struct editmode *prev, *next;
} editmode_t;
 
static editmode_t *mode_tbl = NULL;

static void add_mode(editmode_t *addme)
{
    if (!mode_tbl) {
	mode_tbl = addme;
	addme->prev = addme->next = NULL;
    }
    else {
	editmode_t *it = mode_tbl;
	while (it->next)
	  it = it->next;
	
	it->next = addme;
	addme->next = NULL;
	addme->prev = it;
    }
}

/* register_mode:
 *  Exported as map.register_mode 
 *  Adds new mode into table
 */
void register_mode(char *name, int switchkey, lua_Object table)
{
    if (lua_istable(table)) {
	editmode_t *newone = malloc(sizeof(editmode_t));

	debugprintf("registering: %s\n", name);

	newone->name = strdup(name);
	newone->switchkey = switchkey;
	lua_pushobject(table);
	newone->table_ref = lua_ref(1); 
	add_mode(newone);
    }
}

/* free_mode_table:
 *  Does what its called.
 */
static void free_mode_table()
{
    editmode_t *tmp, *it = mode_tbl;
    while (it) {
	tmp = it->next;

	lua_unref(it->table_ref);
	free(it->name);
	free(it);
	
	it = tmp;
    }
}


/* see if mouse has moved (enough) btn posa and posb */
#define MOVE_THRESHOLD	3
static int moved(int posa, int posb)
{
    int x1, x2, y1, y2;
    
    x1 = posa >> 16;
    x2 = posb >> 16;
    y1 = posa & 0xffff;
    y2 = posb & 0xffff;
    
    if ((ABS(x1-x2) > MOVE_THRESHOLD) || (ABS(y1-y2) > MOVE_THRESHOLD))
      return TRUE;
    else
      return FALSE;   
}


/* to keep the colours correct */
static int _file_select(char *msg, char *path, char *ext)
{
    int r;
    r = file_select(msg, path, ext);
    text_mode(-1);
    return r;
}



/* 
 * calling Lua functions
 * 
 */

    /* -------- example ----------
  a,b = f("how", t.x, 4)

  lua_pushstring("how");                               * 1st argument
  lua_pushobject(lua_getglobal("t"));      * push value of global 't'
  lua_pushstring("x");                          * push the string 'x'
  lua_pushobject(lua_gettable());      * push result of t.x (2nd arg)
  lua_pushnumber(4);                                   * 3rd argument
  lua_callfunction(lua_getglobal("f"));           * call Lua function
  lua_pushobject(lua_getresult(1));   * push first result of the call
  lua_setglobal("a");                      * sets global variable 'a'
  lua_pushobject(lua_getresult(2));   * push second result of the call
  lua_setglobal("b");                      * sets global variable 'b'
    --------------------- */

/* editmode->draw(x,y) */
static void editmode_draw(int table_ref, int a, int b)
{
    lua_Object tbl, f;
    
    tbl = lua_getref(table_ref);
    if (tbl == LUA_NOOBJECT) {
	errprintf("Object was garbage collected!\n");
	return;
    }
      
    lua_pushnumber(a);
    lua_pushnumber(b);
    lua_pushobject(tbl);
    lua_pushstring("draw");
    f = lua_gettable();
    if (!lua_isfunction(f)) {
	errprintf("draw in this lua_Object is not a function!\n");
	return;
    }
    lua_callfunction(f);
}



/* halcyon:
 *  ... + On + On.
 */
static void halcyon()
{
    struct editmode *mode, *tmp;
    int force_draw;
    int x, y, b, p;
    int last_mouse_b = 0, last_mouse_pos = -1, first_mouse_pos = 0;
    
    mx = my = 0;
    
    reset_dirty();
    not_dirty = 0;
    force_draw = palbuf_update = 1;
    
    mode = &mode_tbl[0];
 
    for (;;) 
    {
	/* 
	 * CTRL+key 
	 */
	if (key_shifts & KB_CTRL_FLAG) 
	{
	    /* FIXME- lua these */
	    /* save */
	    if (key[KEY_S])
	    {
		char path[1024] = "";
		if (_file_select("Save .rpx", path, "rpx"))
		{
		    if (save_rpx(path) != 0) {
			alert("Error saving file!", path, "", "Ok", NULL, 13, 27);
		    }
		}
		force_draw = 1;
	    }
	    
	    /* load */
	    else if (key[KEY_L]) 
	    {
		char path[1024] = "";
		if (_file_select("Load .rpx", path, "rpx"))
		{
		    if (load_rpx(path) != 0) {
			alert("Error loading file!", path, "", "Ok", NULL, 13, 27);
		    }
		}
		force_draw = 1;
	    }
	    
	    /* quit! */
	    else if (key[KEY_Q])
	      break;   

	}			       /* end CTRL+key */
	
	/* mode switching */
	tmp = mode_tbl;
	while (tmp) {
	    if (key[tmp->switchkey]) {
		mode = tmp;
		force_draw = 1;
		palbuf_update = 1;
		break;
	    } 
	    tmp = tmp->next;
	}			 
		
	/* inside editing area */
	if (mouse_x < editbuf_w)
	{
	    /* 
	     * scrolling (QASD) 
	     */
	    if (key[KEY_A] && mx > 0) {
		mx--;
		force_draw = 1;
	    }
	    
	    if (key[KEY_D]) {	       /* FIXME - bounds chking */
		mx++;
		force_draw = 1;
	    }
	    
	    if (key[KEY_W] && my > 0) {
		my--;
		force_draw = 1;
	    }
	    
	    if (key[KEY_S]) {	       /* fixme - see above */
		my++;
		force_draw = 1;
	    }
	    
	    /* 
	     * mouse events 
	     */	    
	    x = mx*TILE_W+mouse_x;
	    y = my*TILE_H+mouse_y;
	    
	    b = mouse_b;	       /* XWinAllegro is so slow we have to */
	    p = mouse_pos;	       /* save the values here ... */
	    
	    if (b) {
	
		/* mouse button down */
		if (!last_mouse_b) {
		    //if (mode->mdown)
//		      mode->mdown(x, y, b);
		    first_mouse_pos = p;
		} 
		/* drag */
//		else if (moved(p, last_mouse_pos) && mode->drag) 
  //     	  	  mode->drag(x, y, b);
	    } 
	    else if (last_mouse_b) 
	    {		
		/* clicked */
//		if (!moved(first_mouse_pos, p) && mode->clicked)
//		  mode->clicked(x, y, last_mouse_b);
		
		/* mouse button up */
//		if (mode->mup)
//		  mode->mup(x, y, b);
	    }
	    
	    last_mouse_b = b;
	    last_mouse_pos = p;

	} else {		       /* in palette area */
	    
//	    if (mode->palette_key)
//	      mode->palette_key();
	    
//	    if (mouse_b && mode->palette_click)
//	      mode->palette_click(mouse_x - palette_x, mouse_y, mouse_b);
	}			    
	
	/* update screen */
	if (!not_dirty || force_draw) {
	     
	    if (force_draw)
	      mark_dirty(0, 0, editbuf_w, editbuf_h);
	    
	    clear_dirty(editbuf, NULL);

	    /* draw */
	    tmp = mode_tbl;
	    while (tmp) {
		editmode_draw(tmp->table_ref, mx, my);
		tmp = tmp->next;
	    }
	    
	    /* draw palette */
//	    if (mode->palette_draw)
//	      mode->palette_draw();
	    
	    show_mouse(NULL);    
	    draw_dirty(screen, editbuf);
	    textprintf(screen, font, 0, 0, makecol(0xff,0xff,0xff), "(%d, %d)", mx, my);
	    show_mouse(screen);

	    force_draw = 0;
	} /* end update screen */
	
	/* update palette */
	if (palbuf_update) {
	    show_mouse(NULL);
	    blit(palbuf, screen, 0, 0, palette_x, 0, palbuf->w, palbuf->h);
	    show_mouse(screen);
	    clear(palbuf);
	    palbuf_update = 0;
	}
    
    } /* end forever */
}

#endif 				       /* end obsolete */



/* main:
 *  What else.
 */
int main(int argc, char **argv)
{
    int w = 400, h = 300, bpp = 16;
    
    lua_open();	
    tolua_allegro_open();
    tolua_red_open();

    allegro_init();
    install_keyboard();
    install_timer();
    if (install_mouse() == -1) {
	errprintf("initialising mouse!  Install a mouse driver?\n");
	return 1;
    }
    
    set_color_depth(bpp);
    if (set_gfx_mode(GFX_AUTODETECT, w, h, 0, 0) < 0) {
	errprintf("Couldn't set video mode %dx%dx%dbpp!\n", w, h, bpp);
	return 1;
    }
    
    gui_fg_color = makecol(0,0,0);
    gui_bg_color = makecol(0xff,0xee,0xee);
    
    
    palbuf_w = TILE_W * 2;
    editbuf_w = SCREEN_W - palbuf_w;
    palbuf_h = editbuf_h = SCREEN_H;
    
    editbuf = create_bitmap(editbuf_w, editbuf_h);
    palbuf = create_bitmap(palbuf_w, palbuf_h);

    create_tiles_table();
    reset_rpx();
    
    lua_dofile("scripts/init.lua");
    lua_dofile("scripts/mapeditor.lua");
    
    #if 0
    halcyon();			       /* main loop */
    #endif
    
    destroy_tiles_table();
    destroy_bitmap(editbuf);
    destroy_bitmap(palbuf);
    
    return 0;
}
