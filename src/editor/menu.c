/* menu.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "map.h"
#include "mapfile.h"
#include "editor.h"
#include "editarea.h"


static int prompt (const char *msg, const char *true, const char *false)
{
    gui_fg_color = makecol (0xb0, 0xd0, 0xb0);
    gui_bg_color = makecol (0, 0, 0);
    return (alert (msg, "", "", true, false, 13, 27) == 1);
}


static int prompt_filename (const char *msg, char *path, const char *ext)
{
    BITMAP *bmp;
    int ret;

    bmp = create_bitmap (SCREEN_W, SCREEN_H);
    scare_mouse ();
    blit (screen, bmp, 0, 0, 0, 0, bmp->w, bmp->h);
    unscare_mouse ();

    gui_fg_color = makecol (0xb0, 0xd0, 0xb0);
    gui_bg_color = makecol (0, 0, 0);
    ret = file_select (msg, path, ext);

    scare_mouse ();
    blit (bmp, screen, 0, 0, 0, 0, bmp->w, bmp->h);
    unscare_mouse ();
    destroy_bitmap (bmp);

    return ret;
}


static void new ()
{
    if (prompt ("Erase current map?", "Yes", "No")) {
	map_destroy (map);

	map = map_create ();
	map_resize (map, 64, 64);

	editarea_reset_offset ();
    }
}


static void load ()
{
    char filename[1024] = "";
    map_t *m;

    if (!prompt_filename ("Load...", filename, "pit"))
	return;
    
    m = map_load (filename);
    if (!m)
	prompt ("Error loading map", "Ok", 0);
    else {
	map_destroy (map);
	map = m;
	editarea_reset_offset ();
    }
}


static void save ()
{
    char filename[1024] = "";

    if (prompt_filename ("Save as...", filename, "pit"))
	if (map_save (map, filename) < 0)
	    prompt ("Error saving map", "Ok", 0);
}


static void quit ()
{
    if (prompt ("Are you sure you want to quit?", "Yes", "No"))
	gui_quit ();
}


static void file_menu_slot (ug_widget_t *p, int signal, void *data)
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


void menu_install (int x, int y, int w, int h)
{
    window = gui_window_create (x, y, w, h, GUI_HINT_NOFRAME);
    gui_window_set_depth (window, -1);
    dialog = ug_dialog_create (window, layout, 0);
}


void menu_uninstall ()
{
    ug_dialog_destroy (dialog);
    gui_window_destroy (window);
}
