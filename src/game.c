/* game.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "game.h"
#include "gameloop.h"
#include "map.h"
#include "mapfile.h"
#include "loaddata.h"
#include "luabind.h"
#include "magic4x4.h"
#include "magicld.h"
#include "path.h"
#include "store.h"
#include "vtree.h"


int game (int argc, char *argv[])
{
    /* XXX: somehow share code with editor */
    
    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    /* Initialise.  */
    luabind_init ();
    
    path_init ();
    store_init (201);

    /* Load data.  */
    tiles_load (0);
    lights_load (0);

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
    
    store_shutdown ();
    path_shutdown ();

    luabind_shutdown ();
    
    return 0;
}
