/* menu.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "editarea.h"
#include "editor.h"
#include "map.h"
#include "mapfile.h"


static int prompt (const char *msg, const char *true, const char *false,
		   int truekey, int falsekey)
{
    gui_fg_color = makecol (0xc0, 0xd0, 0xc0);
    gui_bg_color = makecol (0, 0, 0);
    return (alert (msg, "", "", true, false, truekey, falsekey) == 1);
}

static int prompt_filename (const char *msg, char *path, const char *ext, 
			    int path_size)
{
    BITMAP *bmp;
    int ret;

    bmp = create_bitmap (SCREEN_W, SCREEN_H);
    scare_mouse ();
    blit (screen, bmp, 0, 0, 0, 0, bmp->w, bmp->h);
    unscare_mouse ();

    gui_fg_color = makecol (0xc0, 0xd0, 0xc0);
    gui_bg_color = makecol (0, 0, 0);
    ret = file_select_ex (msg, path, ext, path_size, 0, 0);

    scare_mouse ();
    blit (bmp, screen, 0, 0, 0, 0, bmp->w, bmp->h);
    unscare_mouse ();
    destroy_bitmap (bmp);

    return ret;
}


static void new ()
{
    if (prompt ("Erase current map?", "&Yes", "&No", 'y', 'n')) {
	map_destroy (editor_map);

	editor_map = map_create (0);
	map_resize (editor_map, 64, 64);

	editarea_reset_offset ();
    }
}


static void load ()
{
    char filename[1024] = "";
    map_t *m;
    int warning;

    if (!prompt_filename ("Load...", filename, "pit", sizeof filename))
	return;
    
    m = map_load (filename, 1, &warning);
    if (!m)
	prompt ("Error loading map", "&Ok", 0, 'o', 0);
    else {
	if (warning)
	    prompt ("Warnings loading map", "&Ok", 0, 'o', 0);

	map_destroy (editor_map);
	editor_map = m;
	editarea_reset_offset ();
    }
}


static void save ()
{
    char filename[1024] = "";

    if (prompt_filename ("Save as...", filename, "pit", sizeof filename))
	if (map_save (editor_map, filename) < 0)
	    prompt ("Error saving map", "&Ok", 0, 'o', 0);
}


static void quit ()
{
    if (prompt ("Are you sure you want to quit?", "&Yes", "&No", 'y', 'n'))
	gui_quit ();
}


static void file_menu_slot (ug_widget_t *p, ug_signal_t signal, void *data)
{
    void (*proc) () = data;
    if (proc)
	proc ();
}


static ug_menu_item_t file_menu_items[] = {
    { "New",  new },
    { "Load", load },
    { "Save", save },
    { "Quit", quit },
    { 0, 0 }
};

static ug_menu_root_t file_menu = {
    "File",
    file_menu_items
};


static ug_dialog_layout_t layout[] = {
    { &ug_menu, 40, 16, &file_menu, file_menu_slot, 0 },
    { UG_DIALOG_LAYOUT_END }
};


static gui_window_t *window;
static ug_dialog_t *dialog;
static gui_accel_t *accel_load;
static gui_accel_t *accel_save;
static gui_accel_t *accel_quit;


void menu_install (int x, int y, int w, int h)
{
    window = gui_window_create (x, y, w, h, GUI_HINT_NOFRAME);
    gui_window_set_depth (window, -1);
    dialog = ug_dialog_create (window, layout, 0);
    accel_load = gui_accel_create (GUI_ACCEL_CTRL ('l'), load);
    accel_save = gui_accel_create (GUI_ACCEL_CTRL ('s'), save);
    accel_quit = gui_accel_create (GUI_ACCEL_CTRL ('q'), quit);
}


void menu_uninstall ()
{
    gui_accel_destroy (accel_quit);
    gui_accel_destroy (accel_save);
    gui_accel_destroy (accel_load);
    ug_dialog_destroy (dialog);
    gui_window_destroy (window);
}
