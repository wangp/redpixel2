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


/*
 *	Lists of tiles, divided into files (packs)
 */

struct file {
    ed_select_list_t *list;
    int top, selected;
    struct file *prev, *next;
};

static struct file file_list;
static struct file *current;


/* This function was adapted from the Store sources. */
static void add_to_list (ed_select_list_t *list, DATAFILE *d, const char *prefix)
{
    char *name, path[1024];
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
	    add_to_list (list, d[i].dat, path);
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
    add_to_list (f->list, store_file (id), VTREE_TILES);
    
    f->next = file_list.next;
    if (f->next)
	f->next->prev = f;
    f->prev = 0;    
    file_list.next = f;
}


static int make_file_list ()
{
    file_list.next = 0;
    tiles_enumerate (callback);
    return (file_list.next) ? 0 : -1;
}


static void free_file_list ()
{
    struct file *f, *next;

    for (f = file_list.next; f; f = next) {
	next = f->next;
	
	ed_select_list_destroy (f->list);
	free (f);
    }
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

static void change_set (struct file *p)
{
    save_current ();
    current = p;
    selectbar_set_list (current->list);
    restore_current ();
}



/*
 *	Selectbar callbacks
 */
 
static void left_proc ()
{
    struct file *p;
    p = current->prev;
    if (!p) for (p = file_list.next; p->next; p = p->next) ;
    change_set (p);
}

static void right_proc ()
{    
    struct file *p = current->next;
    if (!p) p = file_list.next;
    change_set (p);
}



/*
 *	Mode manager callbacks
 */

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



/*
 *	Editarea callbacks
 */

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
    
    for (f = file_list.next; f; f = f->next) {
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



/*
 *	Module init / shutdown
 */

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
