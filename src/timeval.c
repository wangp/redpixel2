/* timeval.c - struct timeval replacement for those without it
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <time.h>
#include "timeval.h"


#ifdef TARGET_WINDOWS

#include <allegro.h>
#include <winalleg.h>

int gettimeofday (struct pwtimeval *tp, void *unused_tz)
{
    DWORD now = timeGetTime ();
    tp->tv_sec = now / 1000;
    tp->tv_usec = (now % 1000) * 1000;
    return 0;
}

void gettimeofday_init (void)
{
    timeBeginPeriod (1);
}

void gettimeofday_shutdown (void)
{
    timeEndPeriod (1);
}

#endif
