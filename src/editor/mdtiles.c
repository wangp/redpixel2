/* mdtiles.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "depths.h"
#include "editor.h"
#include "editarea.h"
#include "edselect.h"
#include "list.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "map.h"
#include "modemgr.h"
#include "modes.h"
#include "selbar.h"
#include "store.h"


#define DAT_INFO  DAT_ID('i','n','f','o')


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
	if (d[i].type == DAT_INFO)
	    continue;

	name = get_datafile_property (&d[i], DAT_NAME);
	if (!name[0])
	    continue;

	ustrzcpy (path, sizeof path, prefix);
	ustrzcat (path, sizeof path, name);
	
	if (d[i].type == DAT_FILE) {
	    ustrzcat (path, sizeof path, "/");
	    _add_to_list (list, d[i].dat, path);
	}
	else if (d[i].type == DAT_BITMAP)
	    ed_select_list_add_item (list, path, d[i].dat);
    }
}

static void callback (const char *prefix, int id)
{
    struct file *f;
    
    f = alloc (sizeof *f);

    f->list = ed_select_list_create ();
    _add_to_list (f->list, store_file (id), prefix);
    
    list_add (file_list, f);
}

static int make_file_list (void)
{
    list_init (file_list);
    tiles_enumerate (callback);
    return (list_empty (file_list)) ? -1 : 0;
}

static void do_free_file (struct file *f)
{
    ed_select_list_destroy (f->list);
    free (f);
}

static void free_file_list (void)
{
    list_free (file_list, do_free_file);
}


/* Save / restore selectbar state.  */
 
static void save_current (void)
{
    current->top = selectbar_top ();
    current->selected = selectbar_selected ();
}

static void restore_current (void)
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
 
static void left_proc (void)
{
    struct file *prev = current->prev;
    if (prev == (struct file *) &file_list)
	change_set (prev->prev);
    else
	change_set (prev);
}

static void right_proc (void)
{    
    struct file *next = current->next;
    if (next == (struct file *) &file_list)
	change_set (next->next);
    else
	change_set (next);
}


/* Mode manager callbacks.  */

static void enter_mode (void)
{
    editarea_layer_activate ("tiles");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (16, 16);
    selectbar_set_change_set_proc (left_proc, right_proc);
    restore_current ();
}

static void leave_mode (void)
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

    for (y = 0; (y < h) && (y + offy < map_height (editor_map)); y++)
	for (x = 0; (x < w) && (x + offx < map_width (editor_map)); x++) {
	    t = map_tile (editor_map, x + offx, y + offy);
	    if (t)
		draw_magic_sprite (bmp, store[t]->dat, x * 16, y * 16);
	}
}

static int do_tile_set (int x, int y, int tile)
{
    if ((x >= map_width (editor_map)) || (y >= map_height (editor_map)))
	return 0;

    if (map_tile (editor_map, x, y) == tile)
	return 0;
    
    map_set_tile (editor_map, x, y, tile);
    return 1;
}

static void do_tile_pickup (int x, int y)
{
    char *key;
    struct file *f;

    key = store_key (map_tile (editor_map, x, y));
    if (!key) return;
    
    list_for_each (f, &file_list) {
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

int mode_tiles_init (void)
{
    if (make_file_list () < 0)
	return -1;
    current = file_list.next;

    modemgr_register (&tile_mode);
    editarea_layer_register ("tiles", draw_layer, event_layer, DEPTH_TILES);
    return 0;
}

void mode_tiles_shutdown (void)
{
    free_file_list ();
}
