/* 
 * jsgwm.c - default (Allegro-compliant) JSGUI window manager
 */


#include <string.h>
#include <js/jsapi.h>
#include <allegro.h>
#include "jsgui.h"
#include "jsginter.h"


#define WM_TITLEBAR_H	20


#define WM_NORMAL	0
#define WM_ICON		1
#define WM_SHADE	2


typedef struct wm_client
{
    int id;
    int x, y;			       /* includes WM_TITLEBAR_H */
    int w, h;			       
    int status;			       /* WM_NORMAL, ... */

    BITMAP *bitmap;
    int dirty;
    
    struct wm_client *prev, *next;
} wm_client_t;


static BITMAP *dbuf;		       /* double buffer */

static wm_client_t *list_head = NULL;

static someone_dirty = FALSE;	       /* global flag for easy checking */


/* add client to (start of) linked list */
static void link_client(wm_client_t *c)
{
    if (!list_head) {
	list_head = c;
	c->next = c->prev = NULL;
	return;
    }

    c->prev = NULL;
    c->next = list_head;
    list_head->prev = c;
    list_head = c;
}


/* remove client from linked list */
static void unlink_client(wm_client_t *c)
{
    if (c->next) c->next->prev = c->prev;
    if (c->prev) c->prev->next = c->next;
    if (c == list_head)
      list_head = c->next;
}



/* find client in linked list (by index) */
static wm_client_t *get_client(int id)
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
}


static int id_count = 0;

static int new_id() 
{
    if (++id_count > 0xffffff)	       /* FIXME: enough? bad assumption? */
      id_count = 1;
    return id_count;
}


/* create a new client */
static int wm_new_client(int w, int h)
{
    BITMAP *b;
    wm_client_t *c;
    
    b = create_bitmap(w, h + WM_TITLEBAR_H);
    if (!b) 
      return 0;
    
    c = malloc_client();
    if (!c) {
	destroy_bitmap(b);
	return 0;
    }
    
    link_client(c);
    c->id = new_id();
    c->bitmap = b;
    clear(c->bitmap);
    c->w = w;
    c->h = h + WM_TITLEBAR_H;
    c->x = random() % (__gui_width - c->w);
    c->y = random() % (__gui_height - c->h);
    
    return c->id;
}


/* remove a client */
static void wm_remove_client(int id)
{
    wm_client_t *c = get_client(id);
    if (c) {
	unlink_client(c);
	destroy_client(c);
    }
}


/* mark a client for redisplay */
static void wm_mark_client(int id)
{    
    wm_client_t *c = get_client(id);
    if (c) {
	someone_dirty = c->dirty = TRUE;
    }
}


/* draw client onto display */
static inline void draw_client(wm_client_t *c)
{
    jsval argv[2];
    
    argv[0] = INT_TO_JSVAL((int)c->bitmap);
    argv[1] = INT_TO_JSVAL(c->x);
    argv[2] = INT_TO_JSVAL(c->y);
    
    rect(c->bitmap, 0, 0, c->bitmap->w-1, 
	 c->bitmap->h-1, makecol(255,0,0));

    /* FIXME: manual says this is a deprecated function */
    //JS_CallFunctionName(__gui_cx, c->obj, "__draw_self_and_children",
    //3, argv, &result);
    
    blit(c->bitmap, __gui_dest, 0, 0,
	 c->x, c->y, c->w, c->h);
}


/* from PPCol by Ivan Baldo.  Looks like Lisp! */
#define check_collision(x1, y1, w1, h1, x2, y2, w2, h2) \
 	(!(((x1) >= (x2) + (w2)) || ((x2) >= (x1) + (w1)) || \
           ((y1) >= (y2) + (h2)) || ((y2) >= (y1) + (h1))))

/* update what needs to be updated */
static void wm_update()
{
    BITMAP *old_bmp;
    wm_client_t *it;
    int x1=0, y1=0, x2=0, y2=0, w, h;

    if (!someone_dirty)
      return;
    
    old_bmp = __gui_dest;
    __gui_dest = dbuf;
	
    for (it = list_head; it; it = it->next) {
	if (it->dirty) {
	    x1 = MIN(x1, it->x);
	    y1 = MIN(y1, it->y);
	    x2 = MAX(x2, it->x + it->w - 1);
	    y2 = MAX(y2, it->y + it->h - 1);
	    draw_client(it);
	    it->dirty = FALSE;
	}
    }

    __gui_dest = old_bmp;

    w = x2 - x1 + 1;
    h = y2 - y1 + 1;
    
    for (it = list_head; it; it = it->next) {
	if (check_collision(x1, y1, w, h, it->x, it->y, it->w, it->h)) {
	    blit(it->bitmap, dbuf, 0, 0, it->x, it->y, it->w, it->h);
	}
    }
    
    blit(dbuf, __gui_dest, x1, y1, x1, y1, w, h);
    someone_dirty = FALSE;
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
    destroy_bitmap(dbuf);
    while (list_head) 
      wm_remove_client(list_head->id);
}


GUI_WM __gui_default_wm = {
    "JSG WM",
    wm_init,
    wm_die,
    wm_new_client,
    wm_remove_client,
    wm_mark_client,
    wm_update
};
