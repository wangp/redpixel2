/* editor.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "cursor.h"
#include "newfont.h"

#include "editarea.h"
#include "menu.h"
#include "modebar.h"
#include "selbar.h"

#include "modemgr.h"
#include "modes.h"

#include "map.h"


int editor (int argc, char *argv[])
{
    font = &_new_font;

    if ((gui_init () < 0)
	|| (ug_init () < 0)
	|| (cursor_init () < 0))
	return 1;

	editarea_install  (0, 0, SCREEN_W - 32, SCREEN_H - 16);
	selectbar_install (SCREEN_W - 32, 0, 32, SCREEN_H);
	modebar_install   (70, SCREEN_H - 16, SCREEN_W - 70 - 32, 16);
	menu_install      (0, SCREEN_H - 16, 70, 16);

	    modemgr_init ();
	    if ((mode_tiles_init () < 0)
		|| (mode_lights_init () < 0)
		|| (mode_objects_init () < 0)
		|| (mode_starts_init () < 0))
		return 1;
	    modemgr_select ("tiles");

		map = map_create ();
		map_resize (map, 64, 64);

		gui_main ();

		map_destroy (map);
	    
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

    return 0;
}
