/* svstats.c - network stats
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "timeval.h"


static struct pwtimeval count_start_time;

int svstats_incoming_bytes;
int svstats_outgoing_bytes;
float svstats_average_incoming_bytes;
float svstats_average_outgoing_bytes;


void svstats_init (void)
{
    svstats_incoming_bytes =
	svstats_outgoing_bytes = 
	svstats_average_outgoing_bytes =
	svstats_average_incoming_bytes = 0;
    gettimeofday (&count_start_time, 0);
}


int svstats_poll (void)
{
    struct pwtimeval now;
    unsigned long elapsed_msec;

    gettimeofday (&now, 0);
    elapsed_msec = (((now.tv_sec * 1000) + (now.tv_usec / 1000)) -
		    ((count_start_time.tv_sec * 1000) +
		     (count_start_time.tv_usec / 1000)));

    if (elapsed_msec > 1000) {
	svstats_average_incoming_bytes =
	    (float) svstats_incoming_bytes / elapsed_msec * 1000;
	svstats_average_outgoing_bytes =
	    (float) svstats_outgoing_bytes / elapsed_msec * 1000;

	svstats_incoming_bytes = svstats_outgoing_bytes = 0;
	count_start_time = now;

	return 1;
    }

    return 0;
}


void svstats_shutdown (void)
{
}
