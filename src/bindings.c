/* bindings.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "bindings.h"
#include "mylua.h"


/* Bindings we can't generate.  */

#include <allegro.h>
#include "extdata.h"
#include "path.h"
#include "store.h"

static int bind_store_load(LS)
	/* (filename, prefix) : (nil on error) */
{
	const char *filename;
	const char *prefix;
	char **p, tmp[1024];
	if (!checkargs(L, "ss")) goto error;
	filename = tostring(L, 1);
	prefix = tostring(L, 2);
	for (p = path_share; *p; p++) {
	    ustrncpy(tmp, *p, sizeof tmp);
	    ustrncat(tmp, filename, sizeof tmp);
	    if (store_load_ex(tmp, prefix, load_extended_datafile) < 0) goto error;
	    break;
	}
	pushnumber(L, 1);
	return 1;
error:
	pushnil(L);
	return 1;
}


/* Import bindings.  */

#include "object.h"
#include "objtypes.h"


#include "bindings.inc"


/* Module stuff.  */

void bindings_init ()
{
    DO_REGISTRATION(lua_state);

    lregister (lua_state, "store_load", bind_store_load);
}


void bindings_shutdown ()
{
}
