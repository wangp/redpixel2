/* gameinit.c - main init and shutdown
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "bindings.h"
#include "bitmaskr.h"
#include "fastsqrt.h"
#include "gameinit.h"
#include "jpgalleg.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "mylua.h"
#include "object.h"
#include "objtypes.h"
#include "path.h"
#include "store.h"


void game_init ()
{
    build_sqrt_table ();

    generate_magic_color_map ();
    generate_magic_conversion_tables ();
    
    register_bitmap_file_type ("jpg", load_jpg, NULL);

    path_init ();
    store_init (200);
    bitmask_ref_init ();
 
    mylua_open (0);
    bindings_init ();

    tiles_load ();
    lights_load ();
    objtypes_init ();

    object_init ();

    lua_dofile_path (lua_state, "script/init.lua");

    net_init ();
    net_loadconfig (NULL);
}


void game_shutdown ()
{
    net_shutdown ();

    object_shutdown ();

    objtypes_shutdown ();
    lights_unload ();
    tiles_unload ();

    bindings_shutdown ();
    mylua_close ();

    bitmask_ref_shutdown ();
    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
}
