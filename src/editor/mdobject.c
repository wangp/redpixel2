/* mdobject.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "alloc.h"
#include "cursor.h"
#include "depths.h"
#include "editor.h"
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

    list_add (type_list, p);
    return p;
}

static struct type *find_type (const char *name)
{
    struct type *p;

    list_for_each (p, &type_list)
	if (!ustrcmp (name, p->name)) 
	    return p;

    return NULL;
}

static void callback (objtype_t *objtype)
{
    BITMAP *bmp;
    struct type *p;

    if (!objtype_type (objtype))
	return;

    bmp = store_dat (objtype_icon (objtype));
    if (!bmp) return;
    
    p = find_type (objtype_type (objtype));
    if (!p) {
	p = create_type (objtype_type (objtype));
	if (!p) return;
    }

    ed_select_list_add_item (p->list, objtype_name (objtype), bmp);
}

static int make_type_list ()
{
    list_init (type_list);
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
    list_free (type_list, do_free_type);
}


/* Cursor.  */

static int cursor_offset_x;
static int cursor_offset_y;

static void cursor_set_selected ()
{
    BITMAP *bmp;

    bmp = store_dat (objtype_icon (objtypes_lookup (selectbar_selected_name ())));
    if (bmp) {
	cursor_set_magic_bitmap (bmp, 0, 0);
	cursor_offset_x = bmp->w/3/2;
	cursor_offset_y = bmp->h/2;
    }
}


/* Actions box.  */

enum {
    ACTION_ADD,
    ACTION_DEL,
    ACTION_RAISE,
    ACTION_LOWER,
    ACTION_EDIT
};

static gui_window_t *actions_win;
static ug_dialog_t *actions_dlg;
static int action = ACTION_ADD;
static int snap_to_grid = 0;

static void set_cursor_for_action ()
{
    switch (action) {
	case ACTION_ADD:
	    cursor_set_selected ();
	    break;
	case ACTION_DEL:
	    cursor_set_magic_bitmap (store_dat ("/editor/cursor/delete"), 7, 7);
	    break;
	case ACTION_RAISE:
	    cursor_set_magic_bitmap (store_dat ("/editor/cursor/raise"), 3, 0);
	    break;
	case ACTION_LOWER:
	    cursor_set_magic_bitmap (store_dat ("/editor/cursor/lower"), 3, 15);
	    break;
	case ACTION_EDIT:
	    cursor_set_magic_bitmap (store_dat ("/editor/cursor/edit"), 0, 6);
	    break;
    }
}

static void add_slot (ug_widget_t *p, ug_signal_t s, void *d)
{
    action = ACTION_ADD;
    set_cursor_for_action ();
}

static void del_slot (ug_widget_t *p, ug_signal_t s, void *d)
{
    action = ACTION_DEL;
    set_cursor_for_action ();
}

static void raise_slot (ug_widget_t *p, ug_signal_t s, void *d)
{
    action = ACTION_RAISE;
    set_cursor_for_action ();
}

static void lower_slot (ug_widget_t *p, ug_signal_t s, void *d)
{
    action = ACTION_LOWER;
    set_cursor_for_action ();
}

static void edit_slot (ug_widget_t *p, ug_signal_t s, void *d)
{
    action = ACTION_EDIT;
    set_cursor_for_action ();
}

static void snap_slot (ug_widget_t *p, ug_signal_t s, void *d)
{
    snap_to_grid = !snap_to_grid;
}

static ug_dialog_layout_t actions_layout[] =
{
    { &ug_button, -50, 16, "add", add_slot, NULL },
    { &ug_button, -50, 16, "del", del_slot, NULL },
    { UG_DIALOG_LAYOUT_BR },
    { &ug_button, -100, 16, "raise", raise_slot, NULL },
    { UG_DIALOG_LAYOUT_BR },
    { &ug_button, -100, 16, "lower", lower_slot, NULL },
    { UG_DIALOG_LAYOUT_BR },
    { &ug_button, -100, 16, "edit", edit_slot, NULL },
    { UG_DIALOG_LAYOUT_BR },
    { &ug_button, -100, 16, "snap", snap_slot, NULL },
    { UG_DIALOG_LAYOUT_END }
};

static void create_actions_box ()
{
    actions_win = gui_window_create (150, 50, 50, 80, 0);
    gui_window_hide (actions_win);
    gui_window_set_title (actions_win, "obj");
    gui_window_set_alpha (actions_win, 0x70);
    actions_dlg = ug_dialog_create (actions_win, actions_layout, 0);
}

static void destroy_actions_box ()
{
    ug_dialog_destroy (actions_dlg);
    gui_window_destroy (actions_win);
}

static void show_actions_box ()
{
    gui_window_show (actions_win);
}

static void hide_actions_box ()
{
    gui_window_hide (actions_win);
}

static void toggle_actions_box ()
{
    if (gui_window_hidden (actions_win))
	show_actions_box ();
    else
	hide_actions_box ();
}

static int actions_box_hidden ()
{
    return gui_window_hidden (actions_win);
}


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
    set_cursor_for_action ();
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

static int actions_box_was_hidden;

static void enter_mode ()
{
    editarea_layer_activate ("objects");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (32, 32);
    selectbar_set_change_set_proc (left_proc, right_proc);
    selectbar_set_selected_proc (set_cursor_for_action);
    restore_current ();
    set_cursor_for_action ();
    if (!actions_box_was_hidden)
	show_actions_box ();
}

static void leave_mode ()
{
    actions_box_was_hidden = actions_box_hidden ();
    hide_actions_box ();
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
    list_head_t *list;
    object_t *p;

    offx *= 16;
    offy *= 16;

    list = map_object_list (editor_map);

    list_for_each (p, list)
	if (p == highlighted)
	    object_draw_lit_layers (bmp, p, offx, offy, 0x88);
	else
	    object_draw_layers (bmp, p, offx, offy);

    list_for_each (p, list)
	object_draw_lights (bmp, p, offx, offy);
}

static object_t *find_object (int x, int y)
{
    list_head_t *list;
    object_t *p, *last = NULL;
    int x1, y1, x2, y2;

    list = map_object_list (editor_map);
    list_for_each (p, list) {
	object_bounding_box (p, &x1, &y1, &x2, &y2);
	if (in_rect (x, y, object_x (p) + x1, object_y (p) + y1, x2-x1+1, y2-y1+1))
	    last = p;
    }

    return last;
}

static void do_object_pickup (object_t *p)
{
    const char *key;
    struct type *type;

    key = objtype_name (object_type (p));

    list_for_each (type, &type_list) {
	int i = ed_select_list_item_index (type->list, key);
	if (i >= 0) {
	    change_set (type);
	    selectbar_set_selected (i);
	    set_cursor_for_action ();
	    break;
	}
    }
}

static void do_snap (int *x, int *y)
{
    if (snap_to_grid) {
	*x -= *x % editarea_grid_x;
	*y -= *y % editarea_grid_y;
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
	    x = map_x (d->mouse.x);
	    y = map_y (d->mouse.y);
	    p = find_object (x, y);

	    /* LMB */
	    if (d->mouse.b == 0) {
		switch (action) {
		    case ACTION_ADD:
			if (key_shifts & KB_SHIFT_FLAG) {
			    if (p) do_object_pickup (p);
			    move = NULL;
			}
			else if (p) {
			    move = p;
			    if (snap_to_grid) {
				BITMAP *icon = store_dat (objtype_icon (object_type (p)));
				move_offx = -icon->w/3/2;
				move_offy = -icon->h/2;
			    }
			    else {
				move_offx = x - object_x (p);
				move_offy = y - object_y (p);
			    }
			}
			else {
			    p = object_create (selectbar_selected_name ());
			    do_snap (&x, &y);
			    object_set_xy (p, x + cursor_offset_x, y + cursor_offset_y);
			    object_run_init_func (p);
			    map_link_object (editor_map, p);
			    
			    highlighted = move = p;
			    move_offx = x - object_x (p);
			    move_offy = y - object_y (p);
			    cursor_set_dot ();
			    return 1;
			}
			break;

		    case ACTION_DEL:
			if (p) { 
			    map_unlink_object (p);
			    object_destroy (p);
			    return 1;
			}
			break;
			
		    case ACTION_RAISE:
			if (p) {
			    map_unlink_object (p);
			    map_link_object (editor_map, p);
			    return 1;
			}
			break;
			
		    case ACTION_LOWER:
			if (p) {
			    map_unlink_object (p);
			    map_link_object_bottom (editor_map, p);
			    return 1;
			}
			break;
			
		    case ACTION_EDIT:
			if (p) {
			    /* XXX: test only */
			    gui_window_t *w;
			    w = gui_window_create (100, 100, 100, 80, GUI_HINT_GHOSTABLE);
			    gui_window_set_title (w, "Edit object");
			    gui_window_set_alpha (w, 0xa0);
			}
			break;
		}
	    }
	    else if ((d->mouse.b == 1))
		toggle_actions_box ();
	    break;
	    
	case EDITAREA_EVENT_MOUSE_MOVE:
	    x = map_x (d->mouse.x);
	    y = map_y (d->mouse.y);

	    if (move) {
		do_snap (&x, &y);
		x -= move_offx;
		y -= move_offy;
		object_set_xy (move, x, y);
		cursor_set_dot ();
		return 1;
	    }
	    
	    p = find_object (x, y);
	    if (highlighted != p) {
		highlighted = p;
		return 1;
	    }
	    break;

	case EDITAREA_EVENT_MOUSE_UP:
	    set_cursor_for_action ();
	    move = NULL;
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
    
    create_actions_box ();

    modemgr_register (&object_mode);
    editarea_layer_register ("objects", draw_layer, event_layer, DEPTH_OBJECTS);
    
    return 0;
}

void mode_objects_shutdown ()
{
    destroy_actions_box ();
    free_type_list ();
}
