/* game.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "bindings.h"
#include "game.h"
#include "gameloop.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "map.h"
#include "mapfile.h"
#include "objtypes.h"
#include "path.h"
#include "scripts.h"
#include "store.h"
#include "vtree.h"


int game (int argc, char *argv[])
{
    /* XXX: somehow share code with editor */
    
    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    /* Initialise.  */
    path_init ();
    store_init (201);
    scripts_init ();

    tiles_load (0);
    lights_load (0);
    object_types_init ();

    scripts_execute ("script/*.lua");

    /* Load map.  */    
    map = map_load ("test.pit");
    if (!map) {
	allegro_message ("Error loading map.\n");
	goto end;
    }

    /* Do the work.  */
    game_loop ();
    
  end:
    
    /* Shutdown.  */
    if (map)
	map_destroy (map);
    
    object_types_shutdown ();
    lights_unload ();
    tiles_unload ();

    scripts_shutdown ();
    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
	
    return 0;
}
