/* editor.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <lua.h>
#include "gui.h"
#include "ug.h"

#include "bindings.h"
#include "cursor.h"
#include "magic4x4.h"
#include "map.h"
#include "newfont.h"
#include "scripts.h"
#include "store.h"
#include "path.h"

#include "editarea.h"
#include "menu.h"
#include "modemgr.h"
#include "modes.h"
#include "selbar.h"


int editor (int argc, char *argv[])
{
    font = &_new_font;

    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    /* Initialise.  */
    path_init ();
    store_init (201);

    scripts_init ();
    set_object_type_register_hook (mode_objects_object_type_register_hook);
    scripts_execute ("script/*.lua");

    if (gui_init () < 0)
	return 1;
    if (ug_init () < 0)
	return 1;
    cursor_init ();

    editarea_install  (0, 0, SCREEN_W - 32, SCREEN_H - 16);
    selectbar_install (SCREEN_W - 32, 0, 32, SCREEN_H);
    modebar_install   (70, SCREEN_H - 16, SCREEN_W - 70 - 32, 16);
    menu_install      (0, SCREEN_H - 16, 70, 16);

    /* Init mode manager and editing modes.  */
    modemgr_init ();
    if (mode_tiles_init () < 0)
	return 1;
    if (mode_lights_init () < 0)
	return 1;
    if (mode_objects_init () < 0)
	return 1;
    if (mode_starts_init () < 0)
	return 1;
    
    modemgr_select ("tiles");

    map = map_create ();
    map_resize (map, 64, 64);

    /* Do the work.  */
    gui_main ();
    
    /* Shutdown.  */
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
    
    scripts_shutdown ();

    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
    
    return 0;
}
