/* XXX: Replace this with Allegro once it is fixed up.  */

/* yield.c - yeild a timeslice
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <allegro.h>
#include "yield.h"


void yield ()
{
#ifdef ALLEGRO_UNIX

    sleep (0);

#else
    
    yield_timeslice ();

#endif
}
