/* gameinit.c - main init and shutdown
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "bindings.h"
#include "gameinit.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "mylua.h"
#include "objtypes.h"
#include "path.h"
#include "store.h"


void game_init ()
{
    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    path_init ();
    store_init (211);
 
    mylua_open (0);
    bindings_init ();

    tiles_load ();
    lights_load ();
    objtypes_init ();

    dofiles (lua_state, "script/*.lua");
}


void game_shutdown ()
{
    objtypes_shutdown ();
    lights_unload ();
    tiles_unload ();

    bindings_shutdown ();
    mylua_close ();

    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
}
