/* mdtiles.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "depths.h"
#include "editarea.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "map.h"
#include "modemgr.h"
#include "modes.h"
#include "selbar.h"
#include "store.h"
#include "vtree.h"


/* Lists of tiles, divided into files (packs).  */

struct file {
    struct file *next;
    struct file *prev;
    ed_select_list_t *list;
    int top, selected;
};

static struct list_head file_list;
static struct file *current;


/* This function was adapted from the Store sources. */
static void _add_to_list (ed_select_list_t *list, DATAFILE *d, const char *prefix)
{
    const char *name;
    char path[1024];
    int i;

    for (i = 0; d[i].type != DAT_END; i++) {
	name = get_datafile_property (&d[i], DAT_NAME);
	if (!ustrcmp (name, empty_string)
	    || !ustrcmp (name, "GrabberInfo"))
	    continue;

	ustrncpy (path, prefix, sizeof path);
	ustrncat (path, name, sizeof path);
	
	if (d[i].type == DAT_FILE) {
	    ustrncat (path, "/", sizeof path);
	    _add_to_list (list, d[i].dat, path);
	}
	else if (d[i].type == DAT_BITMAP)
	    ed_select_list_add_item (list, path, d[i].dat);
    }
}

static void callback (const char *filename, int id)
{
    struct file *f;
    
    f = alloc (sizeof *f);
    if (!f) return;

    f->list = ed_select_list_create ();
    _add_to_list (f->list, store_file (id), VTREE_TILES);
    
    add_to_list (file_list, f);
}

static int make_file_list ()
{
    init_list (file_list);
    tiles_enumerate (callback);
    return (list_empty (file_list)) ? -1 : 0;
}

static void do_free_file (struct file *f)
{
    ed_select_list_destroy (f->list);
    free (f);
}

static void free_file_list ()
{
    free_list (file_list, do_free_file);
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

static void change_set (struct file *p)
{
    save_current ();
    current = p;
    selectbar_set_list (current->list);
    restore_current ();
}


/* Selectbar callbacks.  */
 
static void left_proc ()
{
    struct file *prev = current->prev;
    if (prev == (struct file *) &file_list)
	change_set (prev->prev);
    else
	change_set (prev);
}

static void right_proc ()
{    
    struct file *next = current->next;
    if (next == (struct file *) &file_list)
	change_set (next->next);
    else
	change_set (next);
}


/* Mode manager callbacks.  */

static void enter_mode ()
{
    editarea_layer_activate ("tiles");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (16, 16);
    selectbar_set_change_set_proc (left_proc, right_proc);
    restore_current ();
}

static void leave_mode ()
{
    save_current ();
    selectbar_set_change_set_proc (0, 0);
    selectbar_set_list (0);
}

static struct editmode tile_mode = {
    "tiles",
    enter_mode,
    leave_mode
};



/* Editarea callbacks.  */

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    int x, y, w, h, t;

    w = (bmp->w / 16) + 1;
    h = (bmp->h / 16) + 1;

    for (y = 0; (y < h) && (y + offy < map->height); y++)
	for (x = 0; (x < w) && (x + offx < map->width); x++) {
	    t = map->tile[y + offy][x + offx];
	    if (t) 
		draw_magic_sprite (bmp, store[t]->dat, x * 16, y * 16);
	}
}

static int do_tile_set (int x, int y, int tile)
{
    if ((x >= map->width) || (y >= map->height))
	return 0;

    if (map->tile[y][x] == tile)
	return 0;
    
    map->tile[y][x] = tile;
    return 1;
}

static void do_tile_pickup (int x, int y)
{
    char *key;
    struct file *f;

    key = store_key (map->tile[y][x]);
    if (!key) return;
    
    foreach (f, file_list) {
	int t = ed_select_list_item_index (f->list, key);
	if (t >= 0) {
	    change_set (f);
	    selectbar_set_selected (t);
	    break;
	}
    }
}

static int event_layer (int event, struct editarea_event *d)
{
    int dirty = 0;
    int x, y;

    x = d->offx + (d->mouse.x / 16);
    y = d->offy + (d->mouse.y / 16);

#define selected  (store_index (selectbar_selected_name ()))
    
    switch (event) {
	case EDITAREA_EVENT_MOUSE_MOVE:
	    if (d->mouse.bstate & 1) {
		if (key_shifts & KB_SHIFT_FLAG)
		    do_tile_pickup (x, y);
		else
		    dirty = do_tile_set (x, y, selected);
	    }
	    else if (d->mouse.bstate & 2)
		dirty = do_tile_set (x, y, 0);
	    break;
	    
	case EDITAREA_EVENT_MOUSE_DOWN:
	    if (d->mouse.b == 0) {
		if (key_shifts & KB_SHIFT_FLAG)
		    do_tile_pickup (x, y);
		else
		    dirty = do_tile_set (x, y, selected);
	    }
	    else if (d->mouse.b == 1)
		dirty = do_tile_set (x, y, 0);
	    break;
    }

    return dirty;

#undef selected
}


/* Module init / shutdown.  */

int mode_tiles_init ()
{
    if (make_file_list () < 0)
	return -1;
    current = file_list.next;

    modemgr_register (&tile_mode);
    editarea_layer_register ("tiles", draw_layer, event_layer, DEPTH_TILES);
    return 0;
}

void mode_tiles_shutdown ()
{
    free_file_list ();
}
