/* gameinit.c - main init and shutdown
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "libnet.h"
#include "bindings.h"
#include "bitmaskr.h"
#include "blod.h"
#include "explo.h"
#include "extdata.h"
#include "fastsqrt.h"
#include "gameinit.h"
#include "jpgalleg.h"
#include "loadaud.h"
#include "loaddata.h"
#include "magic4x4.h"
#include "mylua.h"
#include "object.h"
#include "objtypes.h"
#include "path.h"
#include "store.h"


void game_init (void)
{
    build_sqrt_table ();

    generate_magic_color_map ();
    generate_magic_conversion_tables ();
    
    register_bitmap_file_type ("jpg", load_jpg, NULL);
    register_extended_datafile ();
    register_aud_file_type ();

    path_init ();
    store_init (1024);
    bitmask_ref_init ();
 
    mylua_open ();
    bindings_init ();

    tiles_init ();
    lights_init ();
    objtypes_init ();
    explosion_init ();

    object_init ();

    lua_dofile_path (lua_state, "init.lua");

    blod_init ();
    
    net_init ();
    net_loadconfig (NULL);
    net_detectdrivers (net_drivers_all);
    net_initdrivers (net_drivers_all);
}


void game_shutdown (void)
{
    net_shutdown ();

    blod_shutdown ();

    object_shutdown ();

    explosion_shutdown ();
    objtypes_shutdown ();
    lights_shutdown ();
    tiles_shutdown ();

    bindings_shutdown ();
    mylua_close ();

    bitmask_ref_shutdown ();
    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
}
