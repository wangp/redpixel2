/* bindings.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "bindings.h"
#include "mylua.h"


/* Import bindings.  */

#include <allegro.h>
#include "extdata.h"
#include "gameclt.h"
#include "gamesrv.h"
#include "loaddata.h"
#include "store.h"
#include "object.h"
#include "objtypes.h"


#include "bindings.inc"


/* Module stuff.  */

void bindings_init ()
{
    DO_REGISTRATION(lua_state);
}


void bindings_shutdown ()
{
}
