/* editarea.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <string.h>
#include "gui.h"
#include "editarea.h"
#include "alloc.h"
#include "magic4x4.h"


static gui_window_t *window;
static BITMAP *magic;
static int offsetx, offsety;
static int grid = 1, gridx = 16, gridy = 16;


struct layer {
    char *name;
    void (*draw) (BITMAP *, int, int);
    int (*event) (int, struct editarea_event *);
    int depth;

    int show;
    struct layer *next;
};


static struct layer layers, *active;


static void link_layer (struct layer *q)
{
    struct layer *p;

    for (p = &layers; p; p = p->next) 
	if (!p->next || (p->next && p->next->depth > q->depth)) {
	    q->next = p->next;
	    p->next = q;
	    return;
	}
}

static void unlink_and_free_all_layers ()
{
    struct layer *p, *next;

    for (p = layers.next; p; p = next) {
	next = p->next;
	free (p->name);
	free (p);
    }
}


/*----------------------------------------------------------------------*/

static void dot_grid (BITMAP *bmp, int offx, int offy, int gx, int gy, int col)
{
    int x, y;

    for (y = offy; y < bmp->h; y += gy)
	for (x = offx; x < bmp->w; x += gx)
	    putpixel (bmp, x, y, col);
}


static void draw_layers (BITMAP *bmp)
{
    struct layer *p;

    for (p = layers.next; p; p = p->next)
	if (p->draw && p->show)
	    p->draw (bmp, offsetx, offsety);
}

static void draw (void *p, BITMAP *bmp)
{
    clear (magic);
    draw_layers (magic);
//    set_magic_bitmap_brightness (magic, 0xf, 0xf, 0xf);	/* XXX */
    blit_magic_format (magic, bmp);

    if (grid)
	dot_grid (bmp, 0, 0, gridx, gridy, makecol (0x20, 0xf0, 0xa0));
    rect (bmp, 0, 0, bmp->w - 1, bmp->h - 1, makecol (0x40, 0x40, 0x40));
}


/*----------------------------------------------------------------------*/

static void event (void *p, int event, int d)
{
    struct editarea_event data;
    int dirty = 0;

    /*
     * Handle certain events ourself.
     */
    switch (event) {
	/*  XXX: map boundaries.  */
	case GUI_EVENT_KEY_DOWN:
	case GUI_EVENT_KEY_HOLD: {
	    if (d == KEY_UP)         offsety--, dirty = 1;
	    else if (d == KEY_DOWN)  offsety++, dirty = 1;
	    else if (d == KEY_LEFT)  offsetx--, dirty = 1;
	    else if (d == KEY_RIGHT) offsetx++, dirty = 1;

	    offsetx = MAX (0, offsetx);
	    offsety = MAX (0, offsety);
	    goto skip_layer;
	}
    }    

    /*
     * Pass off other events to active layer.
     */
    if (!active || !active->event)
	goto skip_layer;

    data.offx = offsetx;
    data.offy = offsety;
    data.mouse.x = (gui_mouse.x - gui_window_x (p));
    data.mouse.y = (gui_mouse.y - gui_window_y (p));
    data.mouse.b = 0;
    data.mouse.bstate = mouse_b;

    switch (event) {
	case GUI_EVENT_MOUSE_MOVE:
	    dirty = active->event (EDITAREA_EVENT_MOUSE_MOVE, &data);
	    break;
	case GUI_EVENT_MOUSE_DOWN:
	    data.mouse.b = d;
	    dirty = active->event (EDITAREA_EVENT_MOUSE_DOWN, &data);
	    break;
	case GUI_EVENT_MOUSE_UP:
	    data.mouse.b = d;
	    dirty = active->event (EDITAREA_EVENT_MOUSE_UP, &data);
	    break;
	default:
	    break;
    }

  skip_layer:
    
    if (dirty)
	gui_window_dirty (p);
}


/*----------------------------------------------------------------------*/

void editarea_install (int x, int y, int w, int h)
{
    window = gui_window_create (x, y, w, h, GUI_HINT_NOFRAME | GUI_HINT_STEALFOCUS);
    gui_window_set_depth (window, -1);
    gui_window_set_draw_proc (window, draw);
    gui_window_set_event_proc (window, event);

    magic = create_magic_bitmap (w, h);
    
    offsetx = offsety = 0;

    layers.next = 0;
    active = 0;
}


void editarea_uninstall ()
{
    unlink_and_free_all_layers ();
    destroy_bitmap (magic);
    gui_window_destroy (window);
}


void editarea_layer_register (const char *name, void (*draw) (BITMAP *, int offx, int offy),
			      int (*event) (int event, struct editarea_event *), int depth)
{
    struct layer *p = alloc (sizeof *p);
    
    if (p) {
	p->name = ustrdup (name);
	p->draw = draw;
	p->event = event;
	p->depth = depth;
	p->show = 1;
	link_layer (p);

	if (!active)
	    active = p;
    }
}


void editarea_layer_show (const char *name, int show)
{
    struct layer *p;

    for (p = layers.next; p; p = p->next)
	if (!ustrcmp (p->name, name)) {
	    p->show = show;
	    break;
	}
}


void editarea_layer_activate (const char *name)
{
    struct layer *p;

    for (p = layers.next; p; p = p->next)
	if (!ustrcmp (name, p->name)) {
	    active = p;
	    break;
	}
}
