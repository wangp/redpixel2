/* lights.c - hack of tiles.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "editor.h"
#include "modemgr.h"
#include "editarea.h"
#include "lights.h"
#include "depths.h"
#include "magic4x4.h"
#include "edselect.h"
#include "selbar.h"
#include "store.h"
#include "alloc.h"
#include "rect.h"


static BITMAP *icon;


/*
 * This was copied and modified from tiles.c 
 */

#define BASE_PATH	"/lights/"


struct file {
    char *filename;
    int id;
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
	else if (d[i].type == DAT_BITMAP) {
	    char *name = get_datafile_property (&d[i], DAT_NAME);
	    if ((name) && ustrstr (name, "-icon"))
		ed_select_list_item_add (list, path, d[i].dat);
	}
    }
}

static struct file *load (const char *filename)
{
    int id;
    struct file *f;
    ed_select_list_t *list;

    id = store_load (filename, BASE_PATH);
    if (id < 0) return 0;
    
    f = alloc (sizeof *f);
    if (!f) {
	store_unload (id);
	return 0;
    }

    list = ed_select_list_create ();
    add_to_list (list, store_file (id), BASE_PATH);
    
    f->filename = ustrdup (filename);
    f->id = id;
    f->list = list;

    f->next = file_list.next;
    if (f->next)
	f->next->prev = f;
    f->prev = 0;    
    file_list.next = f;

    return f;
}

static void unload_all ()
{
    struct file *f, *next;

    for (f = file_list.next; f; f = next) {
	next = f->next;
	
	ed_select_list_destroy (f->list);
	store_unload (f->id);
	free (f->filename);
	free (f);
    }
}

/*----------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------*/

static void enter_mode ()
{
    editarea_layer_activate ("lights");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (32, 32);
    selectbar_set_change_set_proc (left_proc, right_proc);
    restore_current ();
} 

static void leave_mode ()
{
    save_current ();
    selectbar_set_change_set_proc (0, 0);
    selectbar_set_list (0);
}

static struct editmode light_mode = {
    "lights",
    enter_mode,
    leave_mode
};

/*----------------------------------------------------------------------*/

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    light_t *p;
    BITMAP *b;

    offx *= 16;
    offy *= 16;
    
    for (p = map->lights.next; p; p = p->next)
	draw_trans_sprite (bmp, icon, 
			   ((p->x - offx) - (icon->w / 6)) * 3,
			    (p->y - offy) - (icon->h / 2));

    for (p = map->lights.next; p; p = p->next) {
	b = store[p->lightmap]->dat;
	draw_trans_sprite(bmp, b,
			  ((p->x - offx) - (b->w / 6)) * 3,
			   (p->y - offy) - (b->h / 2));
    }
}

static light_t *find_light (int x, int y)
{
    light_t *p, *last = 0;
    
    for (p = map->lights.next; p; p = p->next)
	if (in_rect (x, y,
		     p->x - icon->w / 3 / 2,
		     p->y - icon->h / 2,
		     icon->w / 3, icon->h))
	    last = p;

    return last;
}

static int event_layer (int event, struct editarea_event *d)
{
    static light_t *old;    
    light_t *p;
    int x, y;
	
    if (event == EDITAREA_EVENT_MOUSE_DOWN) {
	x = (d->offx * 16) + d->mouse.x;
	y = (d->offy * 16) + d->mouse.y;
	
	if (d->mouse.b == 0) {
	    old = p = find_light (x, y);

	    if (!p) {
		old = map_light_create (map, x, y,
					store_index (selectbar_selected_name ()) - 1);
		return 1;
	    }
	}
	else if (d->mouse.b == 1) {
	    p = find_light (x, y);
	    if (p) {
		map_light_destroy (map, p);
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

/*----------------------------------------------------------------------*/

static void loader (const char *filename, int attrib, int param)
{
    load (filename);
}

int lights_init ()
{
    /* XXX: hardcoded path */
    for_each_file ("../data/lights/*.dat", FA_RDONLY | FA_ARCH, loader, 0);

    if (!file_list.next)
	return -1;
    current = file_list.next;

    modemgr_register (&light_mode);
    editarea_layer_register ("lights", draw_layer, event_layer, DEPTH_LIGHTS);

    /* XXX */ {
	PALETTE pal;
	BITMAP *tmp = load_bitmap ("../data/icons/light16.bmp", pal); /* XXX: path */
	icon = get_magic_bitmap_format (tmp, pal);
	destroy_bitmap (tmp);
    }

    return 0;
}

void lights_shutdown ()
{
    unload_all ();
    destroy_bitmap (icon);
}
