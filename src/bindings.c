/* bindings.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "bindings.h"
#include "mylua.h"


/* Import bindings.  */

#include <allegro.h>
#include "client.h"
#include "explo.h"
#include "extdata.h"
#include "loaddata.h"
#include "svgame.h"
#include "store.h"
#include "object.h"
#include "objtypes.h"


#include "bindings.inc"


/* Module stuff.  */

void bindings_init (void)
{
    DO_REGISTRATION(lua_state);
}


void bindings_shutdown (void)
{
}
