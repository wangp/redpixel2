/* mdlights.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <string.h>
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
#include "path.h"
#include "rect.h"
#include "render.h"
#include "selbar.h"
#include "store.h"
#include "strlcat.h"
#include "strlcpy.h"


#define DAT_INFO  DAT_ID('i','n','f','o')


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
static void _add_to_list (ed_select_list_t *list, DATAFILE *d,
			  const char *prefix)
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

	strlcpy (path, prefix, sizeof path);
	strlcat (path, name, sizeof path);
	
	if (d[i].type == DAT_FILE) {
	    strlcat (path, "/", sizeof path);
	    _add_to_list (list, d[i].dat, path);
	}
	else if (d[i].type == DAT_BITMAP) {
	    const char *name = get_datafile_property (&d[i], DAT_NAME);
	    if ((name) && strstr (name, "-icon"))
		ed_select_list_add_item (list, path, d[i].dat);
	}
    }
}

static void callback (const char *prefix, store_file_t handle)
{
    struct file *f;

    f = alloc (sizeof *f);

    f->list = ed_select_list_create ();
    _add_to_list (f->list, store_get_file (handle), prefix);

    list_add (file_list, f);
}

static int make_file_list (void)
{
    list_init (file_list);
    lights_enumerate (callback);
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
    if (prev == (void *) &file_list)
	change_set (prev->prev);
    else
	change_set (prev);
}

static void right_proc (void)
{    
    struct file *next = current->next;
    if (next == (void *) &file_list)
	change_set (next->next);
    else
	change_set (next);
}


/* Mode manager callbacks.  */

static void enter_mode (void)
{
    editarea_layer_activate ("lights");
    selectbar_set_list (current->list);
    selectbar_set_icon_size (32, 32);
    selectbar_set_change_set_proc (left_proc, right_proc);
    restore_current ();
} 

static void leave_mode (void)
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
    list_head_t *list;
    light_t *p;

    offx *= 16;
    offy *= 16;

    list = map_light_list (editor_map);
    list_for_each (p, list)
	draw_trans_magic_sprite (bmp, icon, 
				 (map_light_x (p) - offx) - (icon->w / 3 / 2),
				 (map_light_y (p) - offy) - (icon->h / 2));

    if (full_brightness)
	set_magic_bitmap_brightness (bmp, 0xf, 0xf, 0xf);
    else
	render_lights (bmp, editor_map, offx, offy);
}

static light_t *find_light (int x, int y)
{
    list_head_t *list;
    light_t *p, *last = 0;

    list = map_light_list (editor_map);
    list_for_each (p, list)
	if (in_rect (x, y,
		     map_light_x (p) - icon->w / 3 / 2,
		     map_light_y (p) - icon->h / 2,
		     icon->w / 3, icon->h))
	    last = p;

    return last;
}

static int event_layer (int event, struct editarea_event *d)
{
    static light_t *old;    
    light_t *p;
    int x, y;

    #define map_x(x)	((d->offx * 16) + (x))
    #define map_y(y)	((d->offy * 16) + (y))

    switch (event) {
	    
	case EDITAREA_EVENT_MOUSE_DOWN:
	    x = map_x (d->mouse.x);
	    y = map_y (d->mouse.y);
	    p = find_light (x, y);
	    
	    if (d->mouse.b == 0) {
		if (p)
		    old = p;
		else {
		    old = map_light_create (editor_map, x, y, store_get_index (selectbar_selected_name ()) - 1);
		    return 1;
		}
	    }
	    else if ((d->mouse.b == 1) && (p)) {
		map_light_destroy (p);
		return 1;
	    }
	    break;
	    
	case EDITAREA_EVENT_MOUSE_MOVE:
	    if (old) {
		map_light_move (old, map_x (d->mouse.x), map_y (d->mouse.y));
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

int mode_lights_init (void)
{
    if (make_file_list () < 0)
	return -1;
    current = file_list.next;
    
    modemgr_register (&light_mode);
    editarea_layer_register ("lights", draw_layer, event_layer, DEPTH_LIGHTS);

    if (!(icon = store_get_dat ("/editor/light-icon")))
	return -1;
    
    return 0;
}

void mode_lights_shutdown (void)
{
    free_file_list ();
}

void mode_lights_toggle (void)
{
    full_brightness = !full_brightness;
}
