/* mdstarts.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "depths.h"
#include "editor.h"
#include "editarea.h"
#include "edselect.h"
#include "list.h"
#include "magic4x4.h"
#include "map.h"
#include "modes.h"
#include "modemgr.h"
#include "path.h"
#include "selbar.h"
#include "rect.h"


static BITMAP *icon;

static ed_select_list_t *list;
static int top, selected;


/* Save / restore selectbar state.  */
 
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


/* Mode manager callbacks.  */

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


/* Editarea callbacks.  */

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    struct list_head *list;
    start_t *p;

    offx *= 16;
    offy *= 16;

    list = map_start_list (editor_map);
    list_for_each (p, list)
	draw_trans_magic_sprite (bmp, icon, 
				 (map_start_x (p) - offx) - (icon->w / 3 / 2),
				 (map_start_y (p) - offy) - (icon->h / 2));
}

static start_t *find_start (int x, int y)
{
    struct list_head *list;
    start_t *p, *last = NULL;

    list = map_start_list (editor_map);
    list_for_each (p, list)
	if (in_rect (x, y, map_start_x (p) - icon->w/3/2, 
		     map_start_y (p) - icon->h/2,
		     icon->w/3, icon->h))
	    last = p;

    return last;
}

static int event_layer (int event, struct editarea_event *d)
{
    static start_t *old;    
    start_t *p;
    int x, y;

    #define map_x(x)	((d->offx * 16) + (x))
    #define map_y(y)	((d->offy * 16) + (y))

    switch (event) {
	    
	case EDITAREA_EVENT_MOUSE_DOWN:
	    x = map_x (d->mouse.x);
	    y = map_y (d->mouse.y);
	    p = find_start (x, y);
	    
	    if (d->mouse.b == 0) {
		if (p)
		    old = p;
		else {
		    old = map_start_create (editor_map, x, y);
		    return 1;
		}
	    }
	    else if ((d->mouse.b == 1) && (p)) {
		map_start_destroy (p);
		return 1;
	    }
	    break;
	    
	case EDITAREA_EVENT_MOUSE_MOVE:
	    if (old) {
		map_start_move (old, map_x (d->mouse.x), map_y (d->mouse.y));
		return 1;
	    }
	    break;

	case EDITAREA_EVENT_MOUSE_UP:
	    old = NULL;
	    break;
    }
    
    return 0;
}


/* Module init / shutdown.  */

static BITMAP *load_start_icon ()
{
    char **path;
    char tmp[1024];
    PALETTE pal;
    BITMAP *bmp;
    BITMAP *icon;
	
    for (path = path_share; *path; path++) {
	ustrzcpy (tmp, sizeof tmp, *path);
	ustrzcat (tmp, sizeof tmp, "misc/start.bmp");

	bmp = load_bitmap (tmp, pal);
	if (bmp) {
	    icon = get_magic_bitmap_format (bmp, pal);
	    destroy_bitmap (bmp);
	    return icon;
	}
    }
    
    return NULL;
}

int mode_starts_init ()
{
    modemgr_register (&start_mode);
    editarea_layer_register ("starts", draw_layer, event_layer, DEPTH_STARTS);

    icon = load_start_icon ();
    if (!icon)
	return -1;

    list = ed_select_list_create ();
    ed_select_list_add_item (list, "start", icon);

    return 0;
}

void mode_starts_shutdown ()
{
    ed_select_list_destroy (list);
    destroy_bitmap (icon);
}
