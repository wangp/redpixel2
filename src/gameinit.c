/* gameinit.c - main init and shutdown
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "libnet.h"
#include "bitmaskr.h"
#include "blod.h"
#include "error.h"
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


#if 0
# define dbg(msg)	puts ("[init] " msg)
#else
# define dbg(msg)
#endif


void game_init (void)
{
    dbg ("build_sqrt_table");
    build_sqrt_table ();

    dbg ("generating magic tables");
    generate_magic_color_map ();
    generate_magic_conversion_tables ();

    dbg ("registering datafile routines");
    register_bitmap_file_type ("jpg", load_jpg, NULL);
    register_extended_datafile ();
    register_aud_file_type ();

    dbg ("initing path, store, bitmask_ref");
    path_init ();
    store_init (1024);
    bitmask_ref_init ();

    dbg ("initing lua");
    if (mylua_open () != 0)
	error ("Error initialising Lua.\n");

    dbg ("initing tiles, lights, objtypes, explosions");
    tiles_init ();
    lights_init ();
    objtypes_init ();
    explosion_init ();

    dbg ("initing object");
    object_init ();

    dbg ("running init.lua");
    if (lua_dofile_path (the_lua_state, "init.lua") != 0)
	error ("Error running init.lua\n");
    dbg ("opening server and client namespaces");
    mylua_open_server_and_client_namespaces ();

    dbg ("initing blod");
    blod_init ();

    dbg ("initing networking");
    net_init ();
    net_loadconfig (NULL);
    net_detectdrivers (net_drivers_all);
    net_initdrivers (net_drivers_all);

    dbg ("done");
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

    mylua_close ();

    bitmask_ref_shutdown ();
    store_shutdown ();
    path_shutdown ();

    free_magic_color_map ();
}
