/* editor.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "newfont.h"
#include "resource.h"
#include "store.h"
#include "magic4x4.h"

#include "editarea.h"
#include "map.h"
#include "modemgr.h"
#include "selbar.h"
#include "tiles.h"
#include "lights.h"


map_t *map;


int editor (int argc, char *argv[])
{
    font = &_new_font;

    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    /* Initialise.  */
    resource_init ();
    store_init (201);
    if (gui_init () < 0)
	return 1;
    if (ug_init () < 0)
	return 1;

    editarea_install (0, 0, SCREEN_W - 32, SCREEN_H - 16);
    selectbar_install (SCREEN_W - 32, 0, 32, SCREEN_H);
    modebar_install (SCREEN_W / 2, SCREEN_H - 16, SCREEN_W / 2 - 32, 16);

    /* Init mode manager and editing modes.  */
    modemgr_init ();
    if (tiles_init () < 0)
	return 1;
    if (lights_init () < 0)
	return 1;
    modemgr_select ("tiles");

    /* Do the work.  */
    map = map_create ();
    map_resize (map, 64, 64);
    gui_main ();
    
    /* Shutdown.  */
    lights_shutdown ();
    tiles_shutdown ();
    modemgr_shutdown ();

    modebar_uninstall ();
    selectbar_uninstall ();
    editarea_uninstall ();

    map_destroy (map);
    
    ug_shutdown ();
    gui_shutdown ();
    
    store_shutdown ();
    resource_shutdown ();

    return 0;
}
