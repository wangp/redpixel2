/* guiwm.c - "window manager"
 * This wm provides a titlebar for frames, with iconify and shade buttons.
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <string.h>
#include <jsapi.h>
#include <allegro.h>
#include "gui.h"
#include "guiinter.h"
#include "guiwm.h"



#define TAINT(client)	(client->dirty = someone_dirty = TRUE)



typedef struct wm_client
{
    int id;
    JSObject *obj;	
    BITMAP *bitmap;
    
    int x, y;			 
    int w, h;
    
    /* for window shading */
    int shaded;
    int sh_real_h;
    
    /* for iconification */
    int iconified;
    BITMAP *ic_bmp;
    int ic_real_x, ic_real_y;
    int ic_real_w, ic_real_h;
    int ic_slot;
    
    int dirty;
    int want_to_die;

    struct wm_client *prev, *next;
} wm_client_t;



static BITMAP *dbuf;		      

static wm_client_t *list_head = NULL;
static wm_client_t *list_tail = NULL;

static int someone_dirty = FALSE;      /* global flag for easy checking */



static char *icon_mask[];

    
    
/* add client to (start of) linked list */
static void link_client(wm_client_t *c)
{
    c->prev = NULL;
    c->next = list_head;
    if (list_head) 
	list_head->prev = c;
    if (!c->next)
	list_tail = c;
    list_head = c;
}



/* add client to end of linked list */
static void link_client_end(wm_client_t *c)
{
    c->prev = list_tail;
    c->next = NULL;
    if (list_tail) 
	list_tail->next = c;
    if (!c->prev)
	list_head = c;
    list_tail = c;
}



/* remove client from linked list */
static void unlink_client(wm_client_t *c)
{
    if (c->next) c->next->prev = c->prev;
    if (c->prev) c->prev->next = c->next;
    if (c == list_head)
      list_head = c->next;
    if (c == list_tail)
	list_tail = c->prev;
    c->next = c->prev = NULL;
}



/* send client to start of list */
static void send_to_front(wm_client_t *c)
{
    unlink_client(c);
    link_client(c);
}


 
/* send client to end of list */
static void send_to_back(wm_client_t *c)
{
    unlink_client(c);
    link_client_end(c);
}


 
/* find client in linked list (by index) */
static inline wm_client_t *get_client(int id)
{
    wm_client_t *it = list_head;
    while (it && it->id != id)
	it = it->next;
    return it;
}



/* allocate memory for a new wm_client */
static wm_client_t *malloc_client()
{
    wm_client_t *c;
    c = malloc(sizeof(wm_client_t));
    if (c) 
      c->bitmap = NULL;
    return c;
}



/* free wm_client_t (assumes unlinked) */
static void destroy_client(wm_client_t *c)
{
    if (c->bitmap)
      free(c->bitmap);
    free(c);
}



static int id_count = 0;

static int new_id() 
{
    if (++id_count > 0xffffff)	   /* should be enough */
      id_count = 1;
    return id_count;
}



/* create a new client */
static int wm_new_client(JSObject *obj, int w, int h)
{
    BITMAP *b;
    wm_client_t *c;
    
    b = create_bitmap(w, h);
    if (!b) 
	return 0;
    clear(b);
    
    c = malloc_client();
    if (!c) 
	return 0;

    link_client(c);
    c->id = new_id();
    c->obj = obj;
    c->bitmap = b;
    c->w = w;
    c->h = h;
    c->x = random() % (__gui_width - c->w);  
    c->y = random() % (__gui_height - c->h);
    c->shaded = FALSE;
    c->iconified = FALSE;
    c->want_to_die = FALSE;
    TAINT(c);
    
    return c->id;
}



/* remove a client */
static void wm_remove_client(int id)
{
    wm_client_t *c = get_client(id);
    if (c) {
	c->want_to_die = TRUE;	       /* don't kill it just yet */
    				       /* let it wait in pain for a while */
	send_to_back(c);
	TAINT(c);
    }
}



/* Can you say ... dirty hack?
 * This is to force an update of a portion of the screen,
 * even if there is no client there (e.g. moved, shaded, etc.)
 */
static void clone_dummy(wm_client_t *c)
{
    wm_client_t *dummy;
    
    dummy = malloc_client();     
    link_client_end(dummy);
    dummy->id = 0;
    dummy->bitmap = create_bitmap(c->w, c->h);
    dummy->x = c->x;
    dummy->y = c->y;
    dummy->w = c->w;
    dummy->h = c->h;
    dummy->dirty = TRUE;
    dummy->want_to_die = TRUE;
}



/* move a client */
static void wm_move_client(int id, int rel_x, int rel_y)
{    
    wm_client_t *c;
    
    c = get_client(id);
    
    if ((c) && ((rel_x != 0) || (rel_y != 0))) {
	clone_dummy(c);
	c->x += rel_x;
	c->y += rel_y;
	TAINT(c);
    }
}



/* mark a client for redisplay */
static void wm_mark_client(int id)
{    
    wm_client_t *c = get_client(id);
    if (c) TAINT(c);
}



/* raise a client */
static void wm_raise_client(int id)
{    
    wm_client_t *c = get_client(id);
    if (c) {
	send_to_front(c);
	TAINT(c);
    }
}



/* lower a client */
static void wm_lower_client(int id)
{    
    wm_client_t *c = get_client(id);
    if (c) {
	send_to_back(c);
	TAINT(c);
    }
}



/* shade a client */
static void shadify(wm_client_t *c)
{
    if (c->shaded) {
	c->h = c->sh_real_h;
	c->shaded = FALSE;
    }
    else {
	clone_dummy(c);
    	c->sh_real_h = c->h;
	c->h = WM_TITLEBAR_H;
	c->shaded = TRUE;
    }

    TAINT(c);
}



#define MAX_SLOT	100

static int slot[MAX_SLOT];



static void draw_icon(BITMAP *b, char *mask[])
{
    int x, y, black, c;
    
    black = makecol(0, 0, 0);

    for (y=0; y<16; y++) {
	for (x=0; x<16; x++) {
	    c = (mask[y][x] == 'X') ? __gui_highlight_colour : black;
	    putpixel(b, x+2, y+2, c);
	}
    }
}



/* iconify a client */
static void iconify(wm_client_t *c)
{
    char *s;
    jsval vp1;
    int i;
	
    clone_dummy(c);

    if (c->iconified) {
	destroy_bitmap(c->bitmap);
	
	c->bitmap = c->ic_bmp;
	c->x = c->ic_real_x;
	c->y = c->ic_real_y;
	c->w = c->ic_real_w;
	c->h = c->ic_real_h;
	slot[c->ic_slot] = FALSE;

    	c->iconified = FALSE;
    }
    else {
	JS_GetProperty(__gui_cx, c->obj, "label", &vp1);
	s = JS_GetStringBytes(JS_ValueToString(__gui_cx, vp1));

	c->ic_bmp = c->bitmap;
	c->ic_real_x = c->x;
	c->ic_real_y = c->y;
	c->ic_real_w = c->w;
	c->ic_real_h = c->h;
	
	for (i=0; i<MAX_SLOT; i++)
	    if (!slot[i])
		break;
	
	c->x = 0;
	c->y = i*20;;
	c->w = text_length(font, s) + 30;
	c->h = WM_TITLEBAR_H;
	c->ic_slot = i;
	slot[i] = TRUE;

	c->bitmap = create_bitmap(c->w, c->h);
	clear(c->bitmap);
	//rect(c->bitmap, 0, 0, c->w-1, c->h-1, __gui_shadow_colour);
	draw_icon(c->bitmap, icon_mask);
	textout(c->bitmap, font, s, 24, 6, __gui_text_colour);

    	c->iconified = TRUE;
    }

    TAINT(c);
}



/* special features of this wm */
static void wm_client_special(int id, int cmd)
{    
    wm_client_t *c = get_client(id);
    if (c) {
	switch (cmd) {
	case 1:			       
	    shadify(c);
	    break;
	case 2:	
	    iconify(c);
	    break;
	}
    }
}



/* draw client onto display */
static inline void draw_client(wm_client_t *c)
{
    jsval vp, argv[2], rval;
    
    argv[1] = argv[0] = INT_TO_JSVAL(0);

    if (JS_GetProperty(__gui_cx, c->obj, "draw_tree", &vp)) {
	/* the manual says this is a deprecated function... */
	JS_CallFunctionValue(__gui_cx, c->obj, vp, 2, argv, &rval);
    }    
}



/* from PPCol by Ivan Baldo.  Looks like Lisp! */
#define check_collision(x1, y1, w1, h1, x2, y2, w2, h2) \
 	(!(((x1) >= (x2) + (w2)) || ((x2) >= (x1) + (w1)) || \
           ((y1) >= (y2) + (h2)) || ((y2) >= (y1) + (h1))))



/* update what needs to be updated */
static void wm_update()
{
    BITMAP *__screen;
    wm_client_t *it;
    int x1 = __gui_width, y1 = __gui_height;
    int x2 = 0, y2 = 0, w, h;
    
    if (!someone_dirty || !list_head)
	return;
    
    /* first get dirty clients to update their own bitmaps */
    __screen = __gui_dest;
	
    for (it = list_head; it; it = it->next) {
	
	if (it->dirty) {
	    x1 = MIN(x1, it->x);
	    y1 = MIN(y1, it->y);
	    x2 = MAX(x2, it->x+it->w-1);
	    y2 = MAX(y2, it->y+it->h-1);
	    
	    if (it->want_to_die) {
		clear(it->bitmap);
	    }
	    else if (!it->iconified) {
		__gui_dest = it->bitmap;
		draw_client(it);
	    }
	    	    
	    it->dirty = FALSE;
	}
    }
    
    /* next blit relevant sections to the double buffer */
    w = x2 - x1 + 1;
    h = y2 - y1 + 1;

    it = list_tail;
    do { 
	if (check_collision(x1, y1, w, h, it->x, it->y, it->w, it->h)) 
	    blit(it->bitmap, dbuf, 0, 0, it->x, it->y, it->w, it->h);

	if (it->want_to_die) {
	    wm_client_t *prev = it->prev;
	    unlink_client(it);
	    destroy_client(it);
	    it = prev;
	}
	else 
	    it = it->prev;
    } while (it);
    
    /* finally blit it to the screen */
    __gui_dest = __screen;
    scare_mouse();
    if (__gui_dest == screen) acquire_screen();
    blit(dbuf, __gui_dest, x1, y1, x1, y1, w, h);
    if (__gui_dest == screen) release_screen();
    unscare_mouse();

    someone_dirty = FALSE;
}



/* return the object in focus (i.e. under mouse) or NULL if not found*/
static JSObject *wm_focus_object(int mx, int my)
{
    wm_client_t *it;
    jsval vp, argv[4], rval;
    
    for (it = list_head; it; it = it->next) {
	
	if (!it->id)
	    continue;
	
	if ((mx >= it->x) && (mx < it->x+it->w) && 
	    (my >= it->y) && (my < it->y+it->h)) {
	    
	    argv[0] = INT_TO_JSVAL(mx - it->x);
	    argv[1] = INT_TO_JSVAL(my - it->y);
	    argv[2] = INT_TO_JSVAL(0);
	    argv[3] = INT_TO_JSVAL(0);
	    
	    if (JS_GetProperty(__gui_cx, it->obj, "focus_object", &vp)) {
		JS_CallFunctionValue(__gui_cx, it->obj, vp, 4, argv, &rval);
		return JSVAL_TO_OBJECT(rval);
	    }
	    
	    return NULL;
	}
    }    
    
    return NULL;
}



/* return the id of the client in focus */
static int wm_focus_client(int mx, int my)
{
    wm_client_t *it;
    
    for (it = list_head; it; it = it->next) {
	
	if (!it->id)
	    continue;
	
	if ((mx >= it->x) && (mx < it->x+it->w) && 
	    (my >= it->y) && (my < it->y+it->h)) {
	    return it->id;
	}
    }    
    
    return 0;
}



/* startup */
static int wm_init(int w, int h, int bpp)
{
    dbuf = create_bitmap_ex(bpp, w, h);
    if (!dbuf) 
	return 0;
    clear(dbuf);
    return 1;
}



/* shutdown */
static void wm_die()
{
    wm_client_t *it, *nx;
    
    destroy_bitmap(dbuf);
    
    it = list_head;
    while (it) {
	nx = it->next;
	destroy_client(it);
	it = nx;
    }
    
    list_head = list_tail = NULL;
}




struct gui_wm __gui_default_wm = {
    "JSGUIWM",
    wm_init,
    wm_die,
    wm_new_client,
    wm_remove_client,
    wm_move_client,
    wm_mark_client,
    wm_raise_client,
    wm_lower_client,
    wm_client_special,
    wm_update,
    wm_focus_object,
    wm_focus_client
};




static char *icon_mask[] = {
    "                ",
    "                ",
    "                ",
    "     XXXXXXXXXX ",
    "     XXXXXXXXXX ",
    "     X        X ",
    "     X        X ",
    "   XXXXXXX    X ",
    "   XXXXXXX    X ",
    "   X     X    X ",
    "   X     X    X ",
    "   X     XXXXXX ",
    "   X     X      ",
    "   XXXXXXX      ",
    "                ",
    "                "
};


