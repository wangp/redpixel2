/* game.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "game.h"
#include "gameloop.h"
#include "map.h"
#include "mapfile.h"


int game (int argc, char *argv[])
{
    map = map_load ("test.pit");
    if (!map) {
	allegro_message ("Error loading map.\n");
	return 1;
    }

    game_loop ();
    
    if (map)
	map_destroy (map);

    return 0;
}
