/* editarea.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <string.h>
#include "gui.h"
#include "alloc.h"
#include "editarea.h"
#include "list.h"
#include "magic4x4.h"
#include "modes.h"
#include "selbar.h"


static gui_window_t *window;
static BITMAP *magic;
static int offsetx, offsety;
static int grid = 1;
int editarea_grid_x = 16;
int editarea_grid_y = 16;


struct layer {
    struct layer *next;
    struct layer *prev;

    char *name;
    void (*draw) (BITMAP *, int, int);
    int (*event) (int, struct editarea_event *);
    int depth;

    int show;
};


static struct list_head layers;
static struct layer *active;


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
    list_for_each (p, &layers) 
	if ((p->draw) && (p->show))
	    p->draw (bmp, offsetx, offsety);
}

static void draw (void *p, BITMAP *bmp)
{
    clear_bitmap (magic);
    draw_layers (magic);
    blit_magic_format (magic, bmp, bmp->w, bmp->h);

    if (grid)
	dot_grid (bmp, 0, 0, editarea_grid_x, editarea_grid_y,
		  makecol (0x40, 0x60, 0x40));
    rect (bmp, 0, 0, bmp->w - 1, bmp->h - 1, makecol (0x40, 0x40, 0x40));
}


/*----------------------------------------------------------------------*/

static void event (void *p, gui_event_t event, int d)
{
    static int panning = 0;
    static int old_x, old_y;
    struct editarea_event data;
    int dirty = 0;

    /*
     * Handle certain events ourself.
     */
    switch (event) {
	/*  XXX: map boundaries.  */
	case GUI_EVENT_KEY_DOWN:
	    if (d == KEY_SPACE) {
		mode_lights_toggle ();
		dirty = 1;
		goto skip_layer;
	    }
	    /* fall through */
	    
	case GUI_EVENT_KEY_HOLD: {
	    if (d == KEY_UP)         offsety--, dirty = 1;
	    else if (d == KEY_DOWN)  offsety++, dirty = 1;
	    else if (d == KEY_LEFT)  offsetx--, dirty = 1;
	    else if (d == KEY_RIGHT) offsetx++, dirty = 1;

	    offsetx = MAX (0, offsetx);
	    offsety = MAX (0, offsety);
	    goto skip_layer;
	}

	case GUI_EVENT_MOUSE_DOWN:
	    if (d == 2) {	/* middle */
		panning = 1;
		old_x = gui_mouse.x;
		old_y = gui_mouse.y;
		gui_mouse_restrict (window);
		scare_mouse ();
		goto skip_layer;
	    }
	    break;

	case GUI_EVENT_MOUSE_MOVE:
	    if (panning) {
		int dx = (gui_mouse.x - old_x) / 4;
		int dy = (gui_mouse.y - old_y) / 4;

		if (dx || dy) {
		    offsetx += dx;
		    offsety += dy;
		    offsetx = MAX (0, offsetx);
		    offsety = MAX (0, offsety);
		    dirty = 1;
		    position_mouse (old_x, old_y);
		}

		goto skip_layer;
	    }
	    break;

	case GUI_EVENT_MOUSE_UP:
	    if (d == 2) {	/* middle */
		if (panning) {
		    unscare_mouse ();
		    gui_mouse_unrestrict ();
		    panning = 0;
		}
		goto skip_layer;
	    }
	    break;

	case GUI_EVENT_MOUSE_WHEEL:
	    if (d > 0)
		selectbar_scroll_up ();
	    else
		selectbar_scroll_down ();
	    goto skip_layer;

	default:
	    break;
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
	case GUI_EVENT_KEY_TYPE:
	    data.key.key = d;
	    dirty = active->event (EDITAREA_EVENT_KEY_TYPE, &data);
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
    window = gui_window_create (x, y, w, h,
				GUI_HINT_NOFRAME | GUI_HINT_STEALFOCUS);
    gui_window_set_depth (window, -1);
    gui_window_set_draw_proc (window, draw);
    gui_window_set_event_proc (window, event);

    magic = create_magic_bitmap (w, h);

    editarea_reset_offset ();

    list_init (layers);
    active = 0;
}


static void free_layer (struct layer *p)
{
    free (p->name);
    free (p);
}


void editarea_uninstall (void)
{
    active = 0;
    list_free (layers, free_layer);
    destroy_bitmap (magic);
    gui_window_destroy (window);
}


void editarea_reset_offset (void)
{
    offsetx = offsety = 0;
    gui_window_dirty (window);
}


static void link_layer (struct layer *layer)
{
    struct layer *p;

    list_for_each (p, &layers)
        if ((p->next == (void *) &layers) ||
	    (p->next->depth > layer->depth)) break;

    list_add_at_pos (p, layer);
}


void editarea_layer_register (const char *name,
			      void (*draw) (BITMAP *, int offx, int offy),
			      int (*event) (int event, struct editarea_event *),
			      int depth)
{
    struct layer *p = alloc (sizeof *p);
    
    p->name = strdup (name);
    p->draw = draw;
    p->event = event;
    p->depth = depth;
    p->show = 1;
    link_layer (p);

    if (!active)
	active = p;
}


void editarea_layer_show (const char *name, int show)
{
    struct layer *p;
    list_for_each (p, &layers)
	if (!strcmp (p->name, name)) {
	    p->show = show;
	    break;
	}
}


void editarea_layer_activate (const char *name)
{
    struct layer *p;
    list_for_each (p, &layers)
	if (!strcmp (name, p->name)) {
	    active = p;
	    break;
	}
}
