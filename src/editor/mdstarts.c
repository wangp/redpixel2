/* mdstarts.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "depths.h"
#include "editarea.h"
#include "edselect.h"
#include "magic4x4.h"
#include "map.h"
#include "modes.h"
#include "modemgr.h"
#include "selbar.h"
#include "rect.h"


static BITMAP *icon;

static ed_select_list_t *list;
static int top, selected;



/*
 *	Save / restore selectbar state
 */
 
static void save_list ()
{
    top = selectbar_top ();
    selected = selectbar_selected ();
}

static void restore_list ()
{
    selectbar_set_top (top);
    selectbar_set_selected (selected);
}



/*
 *	Mode manager callbacks
 */

static void enter_mode ()
{
    editarea_layer_activate ("starts");
    selectbar_set_list (list);
    selectbar_set_icon_size (32, 32);
    restore_list ();
} 

static void leave_mode ()
{
    save_list ();
    selectbar_set_list (0);
}

static struct editmode start_mode = {
    "starts",
    enter_mode,
    leave_mode
};



/*
 *	Editarea callbacks
 */

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    start_t *p;

    offx *= 16;
    offy *= 16;
    
    for (p = map->starts.next; p; p = p->next)
	draw_trans_sprite (bmp, icon, 
			   ((p->x - offx) - (icon->w / 6)) * 3,
			   ((p->y - offy) - (icon->h / 2)));
}

static start_t *find_start (int x, int y)
{
    start_t *p, *last = 0;
    
    for (p = map->starts.next; p; p = p->next)
	if (in_rect (x, y,
		     p->x - icon->w / 3 / 2,
		     p->y - icon->h / 2,
		     icon->w / 3, icon->h))
	    last = p;

    return last;
}

static int event_layer (int event, struct editarea_event *d)
{
    static start_t *old;    
    start_t *p;
    int x, y;
	
    if (event == EDITAREA_EVENT_MOUSE_DOWN) {
	x = (d->offx * 16) + d->mouse.x;
	y = (d->offy * 16) + d->mouse.y;
	
	if (d->mouse.b == 0) {
	    old = p = find_start (x, y);

	    if (!p) {
		old = map_start_create (map, x, y);
		return 1;
	    }
	}
	else if (d->mouse.b == 1) {
	    p = find_start (x, y);
	    if (p) {
		map_start_destroy (map, p);
		return 1;
	    }
	}
    }
    else if (event == EDITAREA_EVENT_MOUSE_MOVE) {
	if (old) {
	    x = (d->offx * 16) + d->mouse.x;
	    y = (d->offy * 16) + d->mouse.y;
	    old->x = x;
	    old->y = y;
	    return 1;
	}
    }
    else if (event ==  EDITAREA_EVENT_MOUSE_UP) {
	old = 0;
    }
    
    return 0;
}



/*
 *	Module init / shutdown
 */

int mode_starts_init ()
{
    modemgr_register (&start_mode);
    editarea_layer_register ("starts", draw_layer, event_layer, DEPTH_STARTS);

    {
	PALETTE pal;
	BITMAP *tmp = load_bitmap ("data/icon/start.bmp", pal); /* XXX: path */
	if (tmp) {
	    icon = get_magic_bitmap_format (tmp, pal);
	    destroy_bitmap (tmp);
	}
    }

    list = ed_select_list_create ();
    ed_select_list_add_item (list, "start", icon);

    return 0;
}

void mode_starts_shutdown ()
{
    ed_select_list_destroy (list);
    destroy_bitmap (icon);
}
