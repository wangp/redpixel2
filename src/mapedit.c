/* mapedit.c : Red Pixel II el-cheapo map editor */

#include <stdio.h>
#include <allegro.h>
#include <seer.h>

#include "defs.h"
#include "script.h"
#include "report.h"
#include "tiles.h"
#include "dirty.h"
#include "mapedit.h"
#include "maptiles.h"
#include "mapobjs.h"
#include "rpx.h"
#include "export.h"



BITMAP *palbuf;			       /* palette buffer */


int top, left;			       /* top left of screen (in tiles) */
int palette_x;			       /* where the icon palette starts */
int screen_w, screen_h;		       /* taking into account palette_x */
int force_draw_palette;


/* edit modes
 */
struct editmode *mode_tbl[] =
{
    &mode_tiles,
    &mode_objects,
    NULL
};


/* halcyon:
 *  ... + On + On.
 */

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
int _file_select(char *msg, char *path, char *ext)
{
    int r;
    r = file_select(msg, path, ext);
    text_mode(-1);
    return r;
}

static void halcyon()
{
    struct editmode *mode, *tmp;
    int i, force_draw;
    int x, y, b, p;
    int last_mouse_b = 0, last_mouse_pos = -1, first_mouse_pos = 0;
    
    top = left = 0;
    
    reset_dirty();
    not_dirty = 0;
    force_draw = force_draw_palette = 1;
    
    mode = mode_tbl[0];
 
    for (;;) 
    {
	/* 
	 * CTRL+key 
	 */
	if (key_shifts & KB_CTRL_FLAG) 
	{

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
	    
	} /* end CTRL+key */
	
	/* mode switching */
	i = 0;
	while ((tmp = mode_tbl[i++])) {
	    if (key[tmp->switchkey]) {
		mode = tmp;
		force_draw = 1;
		force_draw_palette = 1;
		break;
	    }
	}
	
	/* inside editing area */
	if (mouse_x < screen_w)
	{
	    /* 
	     * scrolling (QASD) 
	     */
	    if (key[KEY_A] && left > 0) { 
		left--;
		force_draw = 1;
	    }
	    
	    if (key[KEY_D]) {	    
		left++;
		force_draw = 1;
	    }
	    
	    if (key[KEY_W] && top > 0) {
		top--;
		force_draw = 1;
	    }
	    
	    if (key[KEY_S]) {
		top++;
		force_draw = 1;
	    }
	    
	    /* 
	     * mouse events 
	     */	    
	    x = left*TILE_W+mouse_x;
	    y = top*TILE_H+mouse_y;
	    
	    b = mouse_b;	       /* XWinAllegro is so slow we have to */
	    p = mouse_pos;	       /* save the values here ... */
	    
	    if (b) {
	
		/* mouse button down */
		if (!last_mouse_b) {
		    if (mode->mdown)
		      mode->mdown(x, y, b);
		    first_mouse_pos = p;
		} 
		/* drag */
		else if (moved(p, last_mouse_pos) && mode->drag) 
       	  	  mode->drag(x, y, b);
	    } 
	    else if (last_mouse_b) 
	    {		
		/* clicked */
		if (!moved(first_mouse_pos, p) && mode->clicked)
		  mode->clicked(x, y, last_mouse_b);
		
		/* mouse button up */
		if (mode->mup)
		  mode->mup(x, y, b);
	    }
	    
	    last_mouse_b = b;
	    last_mouse_pos = p;

	} else {		       /* in palette area */
	    
	    if (mode->palette_key)
	      mode->palette_key();
	    
	    if (mouse_b && mode->palette_click)
	      mode->palette_click(mouse_x - palette_x, mouse_y, mouse_b);
	}			    
	
	/* update screen */
	if (!not_dirty || force_draw) {
	     
	    if (force_draw)
	      mark_dirty(0, 0, screen_w, screen_h);
	    
	    clear_dirty(dbuf, NULL);

	    /* draw */
	    i = 0;
	    while ((tmp = mode_tbl[i++])) {
		tmp->draw(left, top);
	    }
	    
	    /* draw palette */
	    if (mode->palette_draw)
	      mode->palette_draw();
	    
	    show_mouse(NULL);    
	    draw_dirty(screen, dbuf);
	    textprintf(screen, font, 0, 0, makecol(0xff,0xff,0xff), "(%d, %d)", left, top);
	    show_mouse(screen);

	    force_draw = 0;
	} /* end update screen */
	
	/* update palette */
	if (force_draw_palette) {
	    show_mouse(NULL);
	    blit(palbuf, screen, 0, 0, palette_x, 0, palbuf->w, palbuf->h);
	    show_mouse(screen);
	    clear(palbuf);
	    force_draw_palette = 0;
	}
    
    } /* end forever */
}


#define LOGFILE	"seer-debug.log"


/* main:
 *  What else.
 */
int main(int argc, char **argv)
{
    int w = 400, h = 300, bpp = 16;

    /* init SeeR debugging */
    scOpen_Debug(LOGFILE);
    scToggle_Debug(true);                                

    /* initialise Allegro */
    allegro_init();
    install_keyboard();
    install_timer();
    if (install_mouse() == -1) {
	consoleprintf("Error calling install_mouse.  Perhaps you need to install a mouse driver?\n");
	return 1;
    }
    
    set_color_depth(bpp);
    if (set_gfx_mode(GFX_AUTODETECT, w, h, 0, 0) < 0) {
	consoleprintf("Couldn't set video mode %dx%dx%dbpp\n", w, h, bpp);
	return 1;
    }
    
    gui_fg_color = makecol(0,0,0);
    gui_bg_color = makecol(0xff,0xee,0xee);

    /* some size-of-screen settings */
    palette_x = SCREEN_W-TILE_W*2;
    screen_w = palette_x;
    screen_h = SCREEN_H;
       
    /* bitmap buffers */
    dbuf = create_bitmap(screen_w, SCREEN_H);
    palbuf = create_bitmap(SCREEN_W - palette_x, SCREEN_H);

    create_tiles_table();

    /* export functions to SeeR scripts */
    export();
    exec_script("scripts/init.sc", "init");
    
    /* clear map */
    reset_rpx();

    /* main loop */
    halcyon();
    
    /* deinit */
    destroy_tiles_table();
    
    destroy_bitmap(dbuf);
    destroy_bitmap(palbuf);
    
    free_scripts();
    scClose_Debug();            
    
    return 0;
}
