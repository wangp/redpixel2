/* blod.c
 *
 * Don't confuse this with blood.c!  "Blod" is Red Pixel terminology
 * for the bigger chunks of red particles that you see.
 *
 * This is only to be run on the client.  All blod objects are proxies.
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include "blod.h"
#include "map.h"
#include "mylua.h"
#include "object.h"
#include "objtypes.h"


static int num_blod_types;	/* inited at run-time */


static struct {
    char *name;
    char *icon;
} blod_type_list[] = {
    { "basic-blod-4x4-000", "/basic/player/blod-4x4/000" },
    { "basic-blod-4x4-001", "/basic/player/blod-4x4/001" },
    { "basic-blod-4x4-002", "/basic/player/blod-4x4/002" },
    { "basic-blod-4x4-003", "/basic/player/blod-4x4/003" },
    { "basic-blod-4x4-004", "/basic/player/blod-4x4/004" },
    { "basic-blod-4x4-005", "/basic/player/blod-4x4/005" },
    { "basic-blod-6x6-000", "/basic/player/blod-6x6/000" },
    { "basic-blod-6x6-001", "/basic/player/blod-6x6/001" },
    { "basic-blod-6x6-002", "/basic/player/blod-6x6/002" },
    { "basic-blod-6x6-003", "/basic/player/blod-6x6/003" },
    { "basic-blod-6x6-004", "/basic/player/blod-6x6/004" },
    { "basic-blod-6x6-005", "/basic/player/blod-6x6/005" },
#if 0
    /* too hugemungous */
    { "basic-blod-8x8-000", "/basic/player/blod-8x8/000" },
    { "basic-blod-8x8-001", "/basic/player/blod-8x8/001" },
    { "basic-blod-8x8-002", "/basic/player/blod-8x8/002" },
    { "basic-blod-8x8-003", "/basic/player/blod-8x8/003" },
    { "basic-blod-8x8-004", "/basic/player/blod-8x8/004" },
    { "basic-blod-8x8-005", "/basic/player/blod-8x8/005" }
#endif
    { 0, 0 }
};


static inline int rnd (int lower, int upper)
{
    return (rand() % (upper-lower+1)) + lower;
}
 

static int blod_update_hook (lua_State *L)
{
    object_set_stale (lua_toobject (L, 1));
    return 0;
}


void blod_spawn (map_t *map, float x, float y, long nparticles)
{
    object_t *obj;
    lua_ref_t hook;

    while (nparticles--) {
	obj = object_create_proxy (blod_type_list[rand () % num_blod_types].name,
				   OBJID_CLIENT_PROCESSED);
	if (!obj)
	    return;

	object_set_xy (obj, x + rnd (-8, 8), y + rnd (-8, 8));
	object_set_mass (obj, rnd (3000, 8000) / 10000000.);
	object_set_collision_flags (obj, 1, 0, 0);

	lua_pushcfunction (lua_state, blod_update_hook);
	hook = lua_ref (lua_state, -1);
	object_set_update_hook (obj, rnd (10 * 1000, 15 * 1000), hook);

	map_link_object (map, obj);
    }
}


void blod_init (void)
{
    int i;
    for (i = 0; blod_type_list[i].name; i++)
	objtypes_register (NULL, blod_type_list[i].name,
			   blod_type_list[i].icon, LUA_NOREF);
    num_blod_types = i;
}


void blod_shutdown (void)
{
}
