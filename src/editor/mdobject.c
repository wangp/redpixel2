/* mdobjects.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <lua.h>
#include "alloc.h"
#include "cursor.h"
#include "depths.h"
#include "editarea.h"
#include "editor.h"
#include "edselect.h"
#include "map.h"
#include "modemgr.h"
#include "modes.h"
#include "selbar.h"
#include "store.h"
#include "path.h"
#include "rect.h"


/*
 *	Lists of objects, divided into types
 */

struct type {
    char *name;
    ed_select_list_t *list;
    int top, selected;
    struct type *prev, *next;
};

static struct type type_list;
static struct type *current;


static struct type *create_type (const char *name)
{
    struct type *p = alloc (sizeof *p);
    if (!p)
	return 0;

    p->name = ustrdup (name);
    p->list = ed_select_list_create ();

    p->next = type_list.next;
    if (p->next)
	p->next->prev = p;
    p->prev = 0;    
    type_list.next = p;

    return p;
}

static struct type *find_type (const char *name)
{
    struct type *p;

    for (p = type_list.next; p; p = p->next)
	if (!ustrcmp (name, p->name))
	    return p;

    return 0;
}

static void add_to_type (struct type *p, char *name, BITMAP *bmp)
{
    ed_select_list_item_add (p->list, name, bmp);
}

static void destroy_all_types ()
{
    struct type *p, *next;

    for (p = type_list.next; p; p = next) {
	next = p->next;
	
	ed_select_list_destroy (p->list);
	free (p->name);
	free (p);
    }
}



static void cursor_set_selected ()
{
    BITMAP *bmp = store_dat (selectbar_selected_name ());
    if (bmp) cursor_set_magic_bitmap (bmp, 0, 0);
}



/*
 *	Save / restore selectbar state
 */
 
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



/*
 *	Selectbar callbacks
 */
 
static void left_proc ()
{
    struct type *p;
    p = current->prev;
    if (!p) for (p = type_list.next; p->next; p = p->next) ;
    change_set (p);
}

static void right_proc ()
{    
    struct type *p = current->next;
    if (!p) p = type_list.next;
    change_set (p);
}



/*
 *	Mode manager callbacks
 */

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



/*
 *	Editarea callbacks
 */

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    object_t *p;
    BITMAP *b;

    offx *= 16;
    offy *= 16;

    for (p = map->objects.next; p; p = p->next) {
	b = store[p->icon]->dat;

	draw_sprite (bmp, b,
		     ((p->x - offx) - (b->w / 6)) * 3,
		     (p->y - offy) - (b->h / 2));
    }
}

static object_t *find_object (int x, int y)
{
    object_t *p, *last = 0;
    BITMAP *b;
    
    for (p = map->objects.next; p; p = p->next) {
	b = store[p->icon]->dat;
	
	if (in_rect (x, y,
		     p->x - (b->w / 6),
		     p->y - (b->h / 2),
		     b->w / 3, b->h))
	    last = p;
    }

    return last;
}

static void do_object_pickup (object_t *p)
{
    char *key;
    struct type *type;

    key = store_key (p->icon);
    if (!key) return;
    
    for (type = type_list.next; type; type = type->next) {
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

    if (event == EDITAREA_EVENT_MOUSE_DOWN) {
	x = (d->offx * 16) + d->mouse.x;
	y = (d->offy * 16) + d->mouse.y;

	if (d->mouse.b == 0) {
	    p = find_object (x, y);

	    move = p;
	    if (move) {
		move_offx = d->mouse.x - (move->x - d->offx * 16);
		move_offy = d->mouse.y - (move->y - d->offy * 16);
	    }

	    if (key_shifts & KB_SHIFT_FLAG) {
		if (p) do_object_pickup (p);
		move = 0;
	    }
	    else if (!p) {
		BITMAP *b;
		
		p = map_object_create (map);
		p->icon = store_index (selectbar_selected_name ());
		b = store[p->icon]->dat;
		p->x = x + (b->w / 6);
		p->y = y + (b->h / 2);

		move = p;
		move_offx = -(b->w / 6);
		move_offy = -(b->h / 2);
		return 1;
	    }
	}
	else if (d->mouse.b == 1) {
	    p = find_object (x, y);
	    if (p) {
		map_object_destroy (map, p);
		return 1;
	    }
	}
    }
    else if (event == EDITAREA_EVENT_MOUSE_MOVE) {
	if (move) {
	    x = (d->offx * 16) + d->mouse.x - move_offx;
	    y = (d->offy * 16) + d->mouse.y - move_offy;
	    move->x = x;
	    move->y = y;
	    cursor_set_dot ();
	    return 1;
	}
    }
    else if (event ==  EDITAREA_EVENT_MOUSE_UP) {
	cursor_set_selected ();
	move = 0;
    }
    
    return 0;
}



/*
 *	Lua export functions
 */

static void __export__register_object (void)
    /* (item-table, type, icon) : (zero on success) */
{
    lua_Object table;
    char *type;
    char *iconname;
    BITMAP *icon;
    struct type *p;

    if ((!lua_istable (lua_getparam (1))
	 && !lua_isnil (lua_getparam (1)))
	|| !lua_isstring (lua_getparam (2))
	|| !lua_isstring (lua_getparam (3)))
	goto error;

    table    = lua_getparam (1);	/* XXX: do something with this */
    type     = lua_getstring (lua_getparam (2));
    iconname = lua_getstring (lua_getparam (3));
    icon = store_dat (iconname);
    if (!icon) goto error;

    p = find_type (type);
    if (!p) {
	p = create_type (type);
	if (!p) goto error;
    }
    
    add_to_type (p, iconname, icon);
    
    lua_pushnumber (0);
    return;
    
  error:

    lua_pushnumber (-1);
}

static void export_functions ()
{
#define e(func)	(lua_register (#func, __export__##func))

    e (register_object);

#undef e
}



/*
 *	Lua interaction
 */

#define INIT		"__module__init"
#define SHUTDOWN	"__module__shutdown"


struct file {
    int shutdown;
    struct file *next;
};

static struct file file_list;


static void do_file (const char *filename, int attrib, int param)
{
    lua_Object func;
    struct file *f;

    lua_pushnil ();
    lua_setglobal (INIT);

    if (lua_dofile ((char *) filename) != 0)
	return;

    func = lua_getglobal (INIT);
    if (lua_isfunction (func))
	lua_callfunction (func);
    
    func = lua_getglobal (SHUTDOWN);
    if (!lua_isfunction (func))
	return;

    f = alloc (sizeof *f);
    if (f) {
	lua_pushobject (func);
	f->shutdown = lua_ref (1);
	f->next = file_list.next;
	file_list.next = f;
    }
}

static void undo_all_files ()
{
    struct file *f, *next;

    for (f = file_list.next; f; f = next) {
	next = f->next;

	lua_callfunction (lua_getref (f->shutdown));
	lua_unref (f->shutdown);
	free (f);
    }
}



/*
 *	Module init / shutdown
 */

int mode_objects_init ()
{
    char **p, tmp[1024];

    export_functions ();
    file_list.next = 0;

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, "scripts/*.lua", sizeof tmp);
	for_each_file (tmp, FA_RDONLY | FA_ARCH, do_file, 0);
    }

    if (!type_list.next)
	return -1;
    current = type_list.next;

    modemgr_register (&object_mode);
    editarea_layer_register ("objects", draw_layer, event_layer, DEPTH_OBJECTS);
    
    return 0;
}

void mode_objects_shutdown ()
{
    undo_all_files ();
    destroy_all_types ();
}
