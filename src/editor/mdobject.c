/* mdobject.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
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

static void add_slot (ug_widget_t *p, int s, void *d) { action = ACTION_ADD; }
static void del_slot (ug_widget_t *p, int s, void *d) { action = ACTION_DEL; }
static void raise_slot (ug_widget_t *p, int s, void *d) { action = ACTION_RAISE; }
static void lower_slot (ug_widget_t *p, int s, void *d) { action = ACTION_LOWER; }
static void edit_slot (ug_widget_t *p, int s, void *d) { action = ACTION_EDIT; }

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
    { UG_DIALOG_LAYOUT_END }
};

static void create_actions_box ()
{
    actions_win = gui_window_create (150, 50, 50, 70, 0);
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

static int actions_box_was_hidden;

static void enter_mode ()
{
    editarea_layer_activate ("objects");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (32, 32);
    selectbar_set_change_set_proc (left_proc, right_proc);
    selectbar_set_selected_proc (cursor_set_selected);
    restore_current ();
    cursor_set_selected ();
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
	if (in_rect (x, y, p->x + x1, p->y + y1, x2-x1+1, y2-y1+1))
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
	    cursor_set_selected ();
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
			    move_offx = x - p->x;
			    move_offy = y - p->y;
			}
			else {
			    p = object_create (selectbar_selected_name ());
			    p->x = x + cursor_offset_x;
			    p->y = y + cursor_offset_y;
			    map_link_object (map, p);
			    
			    highlighted = move = p;
			    move_offx = x - p->x;
			    move_offy = y - p->y;
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
			    map_link_object (map, p);
			    return 1;
			}
			break;
			
		    case ACTION_LOWER:
			if (p) {
			    map_unlink_object (p);
			    map_link_object_bottom (map, p);
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
		move->x = x - move_offx;
		move->y = y - move_offy;
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
	    cursor_set_selected ();
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
