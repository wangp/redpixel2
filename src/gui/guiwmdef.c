/* guiwmdef.c - default [only] window manager
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <string.h>
#include <allegro.h>
#include "gui.h"
#include "guiinter.h"


/* frame dimensions */
#define TOP		17
#define BOTTOM		5
#define LEFT		5
#define RIGHT		5
#define TITLE_X		8
#define TITLE_Y		5


struct gui_window {
    int fx, fy, fw, fh;		/* frame */
    int ux, uy, uw, uh;		/* user */
    int flags, depth;
    int hidden;
    char *title;
    
    void *self;
    void (*draw) (void *, BITMAP *);
    void (*event) (void *, int, int);
    
    BITMAP *fbmp, *ubmp;
    int fdirty, udirty;

    /* shading */
    int shaded;
    int save_w, save_h;
    
    struct gui_window *next;
};


static gui_window_t windows;
static gui_window_t *focus;
static gui_window_t *hasmouse;


static void link_window (gui_window_t *p, gui_window_t *q)
{
    while (p->next) {
	if (p->next->depth > q->depth)
	    break;
	p = p->next;
    }

    q->next = p->next;
    p->next = q;
}

static void unlink_window (gui_window_t *p, gui_window_t *q)
{
    while (p) {
	if (p->next == q) {
	    p->next = q->next;
	    break;
	}
	
	p = p->next;
    }
}


static void send_event (gui_window_t *w, int event, int data)
{
    if ((w) && (w->event))
	w->event (w->self, event, data);
}


/*----------------------------------------------------------------------*/

static int dirty;

static void add_dirty_rect (gui_window_t *w)
{
    gui_dirty_add_rect (w->fx, w->fy, w->fw, w->fh);
    dirty = 1;
}

static void window_frame_dirty (gui_window_t *w)
{
    w->fdirty = dirty = 1;
}

static void window_user_dirty (gui_window_t *w)
{
    w->udirty = dirty = 1;
}


/*----------------------------------------------------------------------*/

static void make_bmps (gui_window_t *w)
{
    w->fbmp = create_bitmap (w->fw, w->fh);
    clear_to_color (w->fbmp, makecol (0, 0, 0));

    if (w->flags & GUI_HINT_NOFRAME)
	w->ubmp = create_sub_bitmap (w->fbmp, 0, 0, w->uw, w->uh);
    else
	w->ubmp = create_sub_bitmap (w->fbmp, LEFT, TOP, w->uw, w->uh);
}


static gui_window_t *window_create (int x, int y, int w, int h, int flags)
{
    gui_window_t *p;

    p = malloc (sizeof *p);
    if (!p) return 0;
    memset (p, 0, sizeof *p);
    
    p->ux = x;
    p->uy = y;
    p->uw = w;
    p->uh = h;
    p->flags = flags;

    if (p->flags & GUI_HINT_NOFRAME) 
	p->fx = x,
	p->fy = y, 
	p->fw = w,
	p->fh = h;
    else 
	p->fx = x - LEFT,
	p->fy = y - TOP,
	p->fw = w + LEFT + RIGHT,
	p->fh = h + TOP + BOTTOM;

    p->self = p;

    make_bmps (p);

    link_window (&windows, p);

    window_frame_dirty (p);
    window_user_dirty (p);

    return p;
}


static void window_destroy (gui_window_t *w)
{
    if (w == focus) {
	send_event (w, GUI_EVENT_WINDOW_UNFOCUSED, 0);
	focus = 0;
    }

    if (w == hasmouse) {
	send_event (w, GUI_EVENT_WINDOW_LOSTMOUSE, 0);
	hasmouse = 0;
    }

    send_event (w, GUI_EVENT_WINDOW_DESTROYED, 0);

    add_dirty_rect (w);

    unlink_window (&windows, w);

    destroy_bitmap (w->ubmp);
    destroy_bitmap (w->fbmp);
    free (w->title);
    free (w);
}


static void window_move (gui_window_t *w, int x, int y)
{
    add_dirty_rect (w);
    
    w->ux = x;
    w->uy = y;

    if (w->flags & GUI_HINT_NOFRAME)
	w->fx = x,
	w->fy = y;
    else 
	w->fx = x - LEFT, 
	w->fy = y - TOP;

    send_event (w, GUI_EVENT_WINDOW_MOVED, 0);

    add_dirty_rect (w);
}


static void window_resize (gui_window_t *p, int w, int h)
{
    add_dirty_rect (p);
        
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    
    p->uw = w;
    p->uh = h;
    
    if (p->flags & GUI_HINT_NOFRAME)
	p->fw = w, 
	p->fh = h;
    else 
	p->fw = w + LEFT + RIGHT,
	p->fh = h + TOP + BOTTOM;
    
    destroy_bitmap (p->ubmp);
    destroy_bitmap (p->fbmp);
    make_bmps (p);

    send_event (p, GUI_EVENT_WINDOW_RESIZED, 0);

    window_frame_dirty (p);
    window_user_dirty (p);
}


static void window_lower (gui_window_t *w)
{
    gui_window_t *p;
    
    unlink_window (&windows, w);
    
    p = &windows;
    while (p->next && (p->next->depth < w->depth)) 
	p = p->next;
    
    w->next = p->next;
    p->next = w;

    send_event (w, GUI_EVENT_WINDOW_LOWERED, 0);

    add_dirty_rect (w);
}


static void window_raise (gui_window_t *w)
{
    gui_window_t *p;
    
    unlink_window (&windows, w);
    
    p = &windows;
    while (p->next && (p->next->depth <= w->depth))
	p = p->next;

    w->next = p->next;
    p->next = w;

    send_event (w, GUI_EVENT_WINDOW_RAISED, 0);

    add_dirty_rect (w);
}


/* XXX: implement window shading as a window manager function
 * (perhaps as an "extended" feature, even though we only have one
 * window manager) */
static void window_shade (gui_window_t *w, int shade)
{
    if (w->flags & GUI_HINT_NOFRAME)
	return;
    if (!!shade == !!w->shaded)
	return;
    
    if (shade) {
	add_dirty_rect (w);
	w->save_w = w->fw;
	w->save_h = w->fh;
	w->fh = TOP;	
    }
    else {
	w->fw = w->save_w;
	w->fh = w->save_h;
    }

    window_frame_dirty (w);
    w->shaded = !!shade;
}


static void window_set_title (gui_window_t *w, const char *title)
{
    free (w->title);
    w->title = ustrdup (title);
    window_frame_dirty (w);
}


static void window_set_depth (gui_window_t *w, int depth)
{
    w->depth = depth;
    unlink_window (&windows, w);
    link_window (&windows, w);
    
    add_dirty_rect (w);
}


static void window_set_self (gui_window_t *w, void *self)
{
    w->self = self;
}


static void window_set_draw_proc (gui_window_t *w, void (*draw) (void *, BITMAP *))
{
    w->draw = draw;
    window_user_dirty (w);
}


static void window_set_event_proc (gui_window_t *w, void (*event) (void *, int, int))
{
    w->event = event;
}

/*----------------------------------------------------------------------*/

static int window_x (gui_window_t *w) { return w->ux; }
static int window_y (gui_window_t *w) { return w->uy; }
static int window_w (gui_window_t *w) { return w->uw; }
static int window_h (gui_window_t *w) { return w->uh; }

/*----------------------------------------------------------------------*/

static void set_focus (gui_window_t *p)
{
    if (focus) {
	send_event (focus, GUI_EVENT_WINDOW_UNFOCUSED, 0);
	window_frame_dirty (focus);
    }
	    
    focus = p;
    if (focus) {
	send_event (focus, GUI_EVENT_WINDOW_FOCUSED, 0);
	window_frame_dirty (focus);
    }
}

static gui_window_t *get_focused (int x, int y)
{
    gui_window_t *w, *z = 0;
    
    for (w = windows.next; w; w = w->next)
	if ((!w->hidden)
	    && (x >= w->fx) && (y >= w->fy) 
	    && (x < w->fx + w->fw) && (y < w->fy + w->fh))
	    z = w;

    return z;
}

static int in_user_area (gui_window_t *w, int x, int y)
{
    return ((x >= w->ux)
	    && (y >= w->uy) 
	    && (x < w->ux + w->uw)
	    && (y < w->uy + w->uh));
}

static int in_title_area (gui_window_t *w, int x, int y)
{
    return ((x >= w->fx) && (x < w->fx + w->fw)
	    && (y >= w->fy) && (y < w->uy));
}

static int in_resize_area (gui_window_t *w, int x, int y)
{
    return (!in_user_area (w, x, y) 
	    && (x > w->fx + w->fw - 5) 
	    && (y > w->fy + w->fh - 5));
}


static int moving, resizing;
static int old_x, old_y;

#define save_old()	(old_x = gui_mouse.x, old_y = gui_mouse.y)

#define MB1	0
#define MB2	1
#define MB3	2


static void wm_handle_event (int event, int d)
{
    gui_window_t *tmp;
    
#if 0
    /* XXX: implement window hiding as a window manager function */
    if (event == GUI_EVENT_KEY_TYPE) {
	gui_window_t *w;
	
	for (w = windows.next; w; w = w->next) 
	    if (!(w->flags & GUI_HINT_NOFRAME)) {
		w->hidden = !w->hidden;
		if (w->hidden && w == focus)
		    focus = 0;
	    }

	dirty = 1;
	return;
    }
#endif

    switch (event) {
	case GUI_EVENT_MOUSE_MOVE:
	    tmp = get_focused (gui_mouse.x, gui_mouse.y);
	
	    if (hasmouse != tmp) {
		send_event (hasmouse, GUI_EVENT_WINDOW_LOSTMOUSE, 0);
		
		hasmouse = tmp;
		send_event (hasmouse, GUI_EVENT_WINDOW_GOTMOUSE, 0);
		
		if (hasmouse && hasmouse->flags & GUI_HINT_STEALFOCUS)
		    set_focus (hasmouse);
	    }
	    break;

	case GUI_EVENT_MOUSE_DOWN:
	    /* click to focus */
	    tmp = get_focused (gui_mouse.x, gui_mouse.y);
	    if (focus != tmp)
		set_focus (tmp);

	    /* raise / lower / shade */
	    if (focus)
		switch (d) {
		    case MB1:
			window_raise (focus);
			break;
		    case MB2:
			if (!in_user_area (focus, gui_mouse.x, gui_mouse.y))
			    window_lower (focus);
			break;
		    case MB3:
			if (in_title_area (focus, gui_mouse.x, gui_mouse.y))
			    window_shade (focus, !focus->shaded);
			break;
		}
	    break;
	    
	case GUI_EVENT_MOUSE_UP:
	    if (d == MB1)
		moving = resizing = 0;
	    break;
	    
	case GUI_EVENT_MOUSE_WHEEL:
	    if (hasmouse && in_title_area (hasmouse, gui_mouse.x, gui_mouse.y))
		window_shade (hasmouse, d > 0);
	    break;
    }
    
    /* the following commands require focus */
    if (!focus)
	goto more;
    
    if ((moving) || (resizing) || (!in_user_area (focus, gui_mouse.x, gui_mouse.y))) {
	switch (event) {
	    case GUI_EVENT_MOUSE_DOWN:
	    	if (d == MB1) {
		    resizing = in_resize_area (focus, gui_mouse.x, gui_mouse.y);
		    moving = !resizing;
		    save_old ();
		}
	    	goto more;

	    case GUI_EVENT_MOUSE_MOVE:
	    	if (moving) 
		    window_move (focus, focus->ux + gui_mouse.x - old_x,
				 	focus->uy + gui_mouse.y - old_y);
	    	else if (resizing) 
		    window_resize (focus, focus->uw + gui_mouse.x - old_x,
				   	  focus->uh + gui_mouse.y - old_y);
		save_old ();
	    	goto more;
	}
    }

    /* chain event to user callback */
    if (in_user_area (focus, gui_mouse.x, gui_mouse.y))
	send_event (focus, event, d);

  more:
    /* nothing yet */
}

/*----------------------------------------------------------------------*/

static void draw_panel (BITMAP *bmp, int x, int y, int w, int h, int invert)
{
    int fg, bg;
    
    /* The Paw colours */
    fg = makecol (0xa0, 0xa0, 0xa0);
    bg = makecol (0x40, 0x40, 0x40);
    
    if (invert) {
	int x = bg;
	bg = fg;
	fg = x;
    }

    hline (bmp, x, y, 	      x + w - 1, fg);
    hline (bmp, x, y + h - 1, x + w - 1, bg);

    vline (bmp, x,         y, y + h - 2, fg);
    vline (bmp, x + w - 1, y, y + h - 1, bg);
}

static void draw_frame (gui_window_t *w)
{
    if (w->flags & GUI_HINT_NOFRAME)
	return;
    
    if (w->title) {
	text_mode (-1);
	textout (w->fbmp, font, w->title, TITLE_X, TITLE_Y, 
		 ((focus == w)
		  ? makecol (0xe0, 0xc0, 0xc0) 
		  : makecol (0xc0, 0xc0, 0xe0)));
    }

    if (!w->shaded) {
	hline (w->fbmp, 0, TOP - 1, w->fw - 1, makecol (0, 0, 0));
	draw_panel (w->fbmp, LEFT - 1, TOP - 1,
		    w->fbmp->w - LEFT - RIGHT + 2,
		    w->fbmp->h - TOP - BOTTOM + 2, 1);
    }

    draw_panel (w->fbmp, 0, 0, w->fw, w->fh, 0);
}

static void draw_user (gui_window_t *w)
{
    if (w->draw)
	w->draw (w->self, w->ubmp);
}

static int wm_update_screen (BITMAP *bmp)
{
    gui_window_t *p;

    if (!dirty) 
	return 0;

    for (p = windows.next; p; p = p->next) {
	if (p->hidden) continue;

	if (p->fdirty) {
	    draw_frame (p);
	    gui_dirty_add_rect (p->fx, p->fy, p->fw, p->fh);
	    p->fdirty = 0;
	}
	
	if (p->udirty) {
	    draw_user (p);
	    if (!p->fdirty) 
		gui_dirty_add_rect (p->ux, p->uy, p->uw, p->uh);
	    p->udirty = 0;
	}
	    
	blit (p->fbmp, bmp, 0, 0, p->fx, p->fy, p->fw, p->fh);
    }

    dirty = 0;

    return 1;
}

/*----------------------------------------------------------------------*/

static void wm_init ()
{
    focus = 0;
}

static void wm_shutdown ()
{
    while (windows.next)
	window_destroy (windows.next);
}

/*----------------------------------------------------------------------*/

gui_wm_t gui_default_wm = {
    wm_init,
    wm_shutdown,
    wm_handle_event,
    wm_update_screen,
    
    window_create,
    window_destroy,
    window_move,
    window_resize,
    window_lower,
    window_raise,
    window_user_dirty,
    window_set_title,
    window_set_depth,
    window_set_self,
    window_set_draw_proc,
    window_set_event_proc,

    window_x,
    window_y,
    window_w,
    window_h
};
