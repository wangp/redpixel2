/* mdlights.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "depths.h"
#include "editarea.h"
#include "edselect.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "map.h"
#include "modemgr.h"
#include "modes.h"
#include "rect.h"
#include "selbar.h"
#include "store.h"
#include "vtree.h"


static int full_brightness = 1;
static BITMAP *icon;


/* Lists of lights, divided into files.  */

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
	else if (d[i].type == DAT_BITMAP) {
	    const char *name = get_datafile_property (&d[i], DAT_NAME);
	    if ((name) && ustrstr (name, "-icon"))
		ed_select_list_add_item (list, path, d[i].dat);
	}
    }
}

static void callback (const char *filename, int id)
{
    struct file *f;

    f = alloc (sizeof *f);

    f->list = ed_select_list_create ();
    _add_to_list (f->list, store_file (id), VTREE_LIGHTS);

    add_to_list (file_list, f);
}

static int make_file_list ()
{
    init_list (file_list);
    lights_enumerate (callback);
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


/* Editarea callbacks.  */

static void draw_layer (BITMAP *bmp, int offx, int offy)
{
    light_t *p;
    BITMAP *b;

    offx *= 16;
    offy *= 16;

    foreach (p, map->lights)
	draw_trans_sprite (bmp, icon, 
			   ((p->x - offx) - (icon->w / 6)) * 3,
			   ((p->y - offy) - (icon->h / 2)));

    if (full_brightness)
	set_magic_bitmap_brightness (bmp, 0xf, 0xf, 0xf);
    else {
	foreach (p, map->lights) {
	    b = store[p->lightmap]->dat;
	    draw_trans_sprite(bmp, b,
			      ((p->x - offx) - (b->w / 6)) * 3,
			      (p->y - offy) - (b->h / 2));
	}
    }
}

static light_t *find_light (int x, int y)
{
    light_t *p, *last = 0;
    
    foreach (p, map->lights)
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
		map_light_destroy (p);
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


/* Module init / shutdown.  */

int mode_lights_init ()
{
    if (make_file_list () < 0)
	return -1;
    current = file_list.next;
    
    modemgr_register (&light_mode);
    editarea_layer_register ("lights", draw_layer, event_layer, DEPTH_LIGHTS);

    {
	PALETTE pal;
	BITMAP *tmp = load_bitmap ("data/icon/light.bmp", pal); /* XXX: path */
	if (tmp) {
	    icon = get_magic_bitmap_format (tmp, pal);
	    destroy_bitmap (tmp);
	}
    }

    return 0;
}

void mode_lights_shutdown ()
{
    free_file_list ();
    destroy_bitmap (icon);
}

void mode_lights_toggle ()
{
    full_brightness = !full_brightness;
}
