/* yield.c - yield a timeslice
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <sys/time.h>
#include <allegro.h>
#include "yield.h"


void yield (void)
{
#ifdef ALLEGRO_UNIX

    /* Under Unix we use select() instead of the sched_yield() that
     * Allegro uses for a stronger effect.  */
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    select(0, 0, 0, 0, &tv);

#else
#ifdef ALLEGRO_WINDOWS
    
    /* This seems to give better results under Windows that Sleep(0)
     * (which is what yield_timeslice() uses).  */

    void __stdcall Sleep (int);
    Sleep (5);

#else

    yield_timeslice ();

#endif
#endif
}
