/* mdobject.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "cursor.h"
#include "depths.h"
#include "editarea.h"
#include "edselect.h"
#include "list.h"
#include "map.h"
#include "modemgr.h"
#include "modes.h"
#include "selbar.h"
#include "store.h"
#include "object.h"
#include "objtypes.h"
#include "rect.h"


/* Lists of objects, divided into types.  */

struct type {
    struct type *next;
    struct type *prev;
    char *name;
    ed_select_list_t *list;
    int top, selected;
};

static struct list_head type_list;
static struct type *current;


static struct type *create_type (const char *name)
{
    struct type *p = alloc (sizeof *p);

    p->name = ustrdup (name);
    p->list = ed_select_list_create ();

    add_to_list (type_list, p);
    return p;
}

static struct type *find_type (const char *name)
{
    struct type *p;

    foreach (p, type_list)
	if (!ustrcmp (name, p->name)) 
	    return p;

    return NULL;
}

static void callback (objtype_t *objtype)
{
    BITMAP *bmp;
    struct type *p;

    bmp = store_dat (objtype->icon);
    if (!bmp) return;
    
    p = find_type (objtype->type);
    if (!p) {
	p = create_type (objtype->type);
	if (!p) return;
    }

    ed_select_list_add_item (p->list, objtype->name, bmp);
}

static int make_type_list ()
{
    init_list (type_list);
    objtypes_enumerate (callback);
    return (list_empty (type_list)) ? -1 : 0;
}

static void do_free_type (struct type *p)
{
    ed_select_list_destroy (p->list);
    free (p->name);
    free (p);
}

static void free_type_list ()
{
    free_list (type_list, do_free_type);
}


/* Cursor.  */

static int cursor_offset_x;
static int cursor_offset_y;

static void cursor_set_selected ()
{
    BITMAP *bmp;

    bmp = store_dat (objtypes_lookup (selectbar_selected_name ())->icon);
    if (bmp) {
	cursor_set_magic_bitmap (bmp, 0, 0);
	cursor_offset_x = bmp->w/3/2;
	cursor_offset_y = bmp->h/2;
    }
}

#define cursor_x(x)	((x) + cursor_offset_x)
#define cursor_y(y)	((y) + cursor_offset_y)


/* Save / restore selectbar state.  */
 
static void save_current ()
{
    current->top = selectbar_top ();
    current->selected = selectbar_selected ();
}

static void restore_current ()
{
    selectbar_set_top (current->top);
    selectbar_set_selected (current->selected);
}

static void change_set (struct type *p)
{
    save_current ();
    current = p;
    selectbar_set_list (current->list);
    restore_current ();
    cursor_set_selected ();
}


/* Selectbar callbacks.  */
 
static void left_proc ()
{
    struct type *prev = current->prev;
    if (prev == (struct type *) &type_list)
	change_set (prev->prev);
    else
	change_set (prev);
}

static void right_proc ()
{    
    struct type *next = current->next;
    if (next == (struct type *) &type_list)
	change_set (next->next);
    else
	change_set (next);
}


/* Mode manager callbacks.  */

static void enter_mode ()
{
    editarea_layer_activate ("objects");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (32, 32);
    selectbar_set_change_set_proc (left_proc, right_proc);
    selectbar_set_selected_proc (cursor_set_selected);
    restore_current ();
    cursor_set_selected ();
}

static void leave_mode ()
{
    cursor_set_default ();
    save_current ();
    selectbar_set_selected_proc (0);
    selectbar_set_change_set_proc (0, 0);
    selectbar_set_list (0);
}

static struct editmode object_mode = {
    "objects",
    enter_mode,
    leave_mode
};


/* Editarea callbacks.  */

static object_t *highlighted;

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    object_t *p;

    offx *= 16;
    offy *= 16;

    foreach (p, map->objects)
	if (p == highlighted)
	    object_draw_lit_layers (bmp, p, offx, offy, 0x88);
	else
	    object_draw_layers (bmp, p, offx, offy);

    foreach (p, map->objects)
	object_draw_lights (bmp, p, offx, offy);
}

static object_t *find_object (int x, int y)
{
    object_t *p, *last = NULL;
    int x1, y1, x2, y2;
    
    foreach (p, map->objects) {
	object_bounding_box (p, &x1, &y1, &x2, &y2);
	if (in_rect (x, y, p->cvar.x + x1, p->cvar.y + y1, x2-x1+1, y2-y1+1))
	    last = p;
    }

    return last;
}

static void do_object_pickup (object_t *p)
{
    const char *key;
    struct type *type;

    key = p->type->name;

    foreach (type, type_list) {
	int i = ed_select_list_item_index (type->list, key);
	if (i >= 0) {
	    change_set (type);
	    selectbar_set_selected (i);
	    break;
	}
    }
}

static int event_layer (int event, struct editarea_event *d)
{
    static object_t *move;
    static int move_offx, move_offy;
    object_t *p;
    int x, y;

    #define map_x(x)	((d->offx * 16) + (x))
    #define map_y(y)	((d->offy * 16) + (y))

    switch (event) {

	case EDITAREA_EVENT_MOUSE_DOWN:
	    x = map_x (cursor_x (d->mouse.x));
	    y = map_y (cursor_y (d->mouse.y));
	    p = find_object (x, y);

	    if (d->mouse.b == 0) {
		if (key_shifts & KB_SHIFT_FLAG) {
		    if (p) do_object_pickup (p);
		    move = NULL;
		}
		else if (key_shifts & KB_CTRL_FLAG) {
		    if (p) {
			map_unlink_object (p);
			map_link_object (map, p);
			return 1;
		    }
		}
		else if (p) {
		    move = p;
		    move_offx = map_x (cursor_x (d->mouse.x)) - p->cvar.x;
		    move_offy = map_y (cursor_y (d->mouse.y)) - p->cvar.y;
		}
		else {
		    p = object_create (selectbar_selected_name ());
		    p->cvar.x = x;
		    p->cvar.y = y;
		    map_link_object (map, p);

		    highlighted = move = p;
		    move_offx = move_offy = 0;
		    cursor_set_dot ();
		    return 1;
		}
	    }
	    else if ((d->mouse.b == 1) && (p)) {
		map_unlink_object (p);
		if (key_shifts & KB_CTRL_FLAG)
		    map_link_object_bottom (map, p);
		else
		    object_destroy (p);
		return 1;
	    }
	    break;
	    
	case EDITAREA_EVENT_MOUSE_MOVE:
	    if (move) {
		x = map_x (cursor_x (d->mouse.x)) - move_offx;
		y = map_y (cursor_y (d->mouse.y)) - move_offy;
		move->cvar.x = x;
		move->cvar.y = y;
		cursor_set_dot ();
		return 1;
	    }
	    
	    p = find_object (map_x (cursor_x (d->mouse.x)),
			     map_y (cursor_y (d->mouse.y)));
	    if (highlighted != p) {
		highlighted = p;
		return 1;
	    }
	    break;

	case EDITAREA_EVENT_MOUSE_UP:
	    cursor_set_selected ();
	    move = 0;
	    break;
    }

    return 0;
}


/* Module init / shutdown.  */

int mode_objects_init ()
{
    if (make_type_list () < 0)
	return -1;
    current = type_list.next;

    modemgr_register (&object_mode);
    editarea_layer_register ("objects", draw_layer, event_layer, DEPTH_OBJECTS);
    
    return 0;
}

void mode_objects_shutdown ()
{
    free_type_list ();
}
