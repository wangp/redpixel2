/* gameinit.c - main init and shutdown
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gameinit.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "objtypes.h"
#include "path.h"
#include "scripts.h"
#include "store.h"


void game_init ()
{
    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    path_init ();
    store_init (201);
    scripts_init ();

    tiles_load ();
    lights_load ();
    object_types_init ();

    scripts_execute ("script/*.lua");
}


void game_shutdown ()
{
    object_types_shutdown ();
    lights_unload ();
    tiles_unload ();

    scripts_shutdown ();
    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
}
