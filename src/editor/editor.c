/* editor.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "cursor.h"
#include "extdata.h"
#include "store.h"
#include "path.h"

#include "editarea.h"
#include "menu.h"
#include "modebar.h"
#include "selbar.h"

#include "modemgr.h"
#include "modes.h"

#include "map.h"


static store_file_t support_file;
map_t *editor_map;


int editor_init (void)
{
    support_file = store_load_ex ("data/editor/editor-support.dat", "/editor/",
				  load_extended_datafile);
    return (support_file) ? 0 : -1;
}


void editor_shutdown (void)
{
    store_unload (support_file);
}


int editor_run (void)
{
    FONT *old_font = font;

    font = store_get_dat ("/editor/font");

    if ((gui_init () < 0)
	|| (ug_init () < 0)
	|| (cursor_init () < 0))
	goto error;

	editarea_install  (0, 0, SCREEN_W - 32, SCREEN_H - 16);
	selectbar_install (SCREEN_W - 32, 0, 32, SCREEN_H);
	modebar_install   (70, SCREEN_H - 16, SCREEN_W - 70 - 32, 16);
	menu_install      (0, SCREEN_H - 16, 70, 16);

	    modemgr_init ();
	    if ((mode_tiles_init () < 0)
		|| (mode_lights_init () < 0)
		|| (mode_objects_init () < 0)
		|| (mode_starts_init () < 0))
		goto error;
	    modemgr_select ("tiles");

		editor_map = map_create (0);
		map_resize (editor_map, 64, 64);

		gui_main ();

		map_destroy (editor_map);
	    
	    mode_starts_shutdown ();
	    mode_objects_shutdown ();
	    mode_lights_shutdown ();
	    mode_tiles_shutdown ();
	    modemgr_shutdown ();

	menu_uninstall ();
	modebar_uninstall ();
	selectbar_uninstall ();
	editarea_uninstall ();
    
    cursor_shutdown ();
    ug_shutdown ();
    gui_shutdown ();

    font = old_font;
    return 0;

  error:

    font = old_font;
    return 1;
}
