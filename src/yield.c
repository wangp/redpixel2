/* XXX: Replace this with Allegro once it is fixed up.  */

/* yield.c - yeild a timeslice
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <sys/time.h>
#include <allegro.h>
#include "yield.h"


void yield ()
{
#ifdef ALLEGRO_UNIX
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    select(0, 0, 0, 0, &tv);

#else
    
    yield_timeslice ();

#endif
}
