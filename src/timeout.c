/* timeout.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <unistd.h>
#include <sys/time.h>
#include "alloc.h"
#include "timeout.h"


void timeout_set (timeout_t *timeout, unsigned long msec)
{
    struct timeval now;

    gettimeofday (&now, NULL);
    timeout->timeout.tv_sec = now.tv_sec + (msec / 1000);
    timeout->timeout.tv_usec = now.tv_usec + ((msec % 1000) * 1000);
}


int timeout_test (timeout_t *timeout)
{
    struct timeval now;

    gettimeofday (&now, NULL);

    return ((now.tv_sec > timeout->timeout.tv_sec) ||
	    ((now.tv_sec == timeout->timeout.tv_sec) &&
	     (now.tv_usec > timeout->timeout.tv_usec)));
}
