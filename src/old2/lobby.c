/* lobby.c (not yet)
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "gamenet.h"
#include "lobby.h"


int lobby_init ()
{
    game_started = 0;
    return 0;
}


void lobby_shutdown ()
{
}


int lobby_update (BITMAP *dbuf)
{
    textout_centre (dbuf, font, "I'm in the lobby", dbuf->w / 6, dbuf->h / 2, makecol24 (0xaa, 0xaa, 0xff));

    if (game_started)
	return 1;
    else
	return 0;
}
