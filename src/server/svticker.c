/* svticker.c - ticker using gettimeofday
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <inttypes.h>
#include "alloc.h"
#include "svticker.h"
#include "timeval.h"


struct svticker {
    unsigned int msecs_per_tick;
    uint32_t ticks;
    struct pwtimeval last_update;
};


svticker_t *svticker_create (unsigned int ticks_per_sec)
{
    svticker_t *t = alloc (sizeof *t);

    t->msecs_per_tick = 1000 / ticks_per_sec;
    t->ticks = 0;
    gettimeofday (&t->last_update, NULL);

    return t;
}


int svticker_poll (svticker_t *t)
{
    struct pwtimeval now;
    unsigned long elapsed_msec;

    gettimeofday (&now, NULL);

    elapsed_msec = (((now.tv_sec * 1000) + (now.tv_usec / 1000)) -
		    ((t->last_update.tv_sec * 1000) +
		     (t->last_update.tv_usec / 1000)));

    if (elapsed_msec > t->msecs_per_tick) {
	long x;

	t->ticks += elapsed_msec / t->msecs_per_tick;
	x = elapsed_msec % t->msecs_per_tick;
	t->last_update = now;
	t->last_update.tv_sec -= x / 1000;
	t->last_update.tv_usec -= x * 1000;
	return 1;
    }

    return 0;
}


void svticker_destroy (svticker_t *t)
{
    free (t);
}


uint32_t svticker_ticks (svticker_t *t)
{
    return t->ticks;
}
