/* netmain.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "net.h"
#include "netinter.h"


NET_DRIVERNAME *net_drivers;


volatile int net_time;

static void net_time_incrementor ()
{
    net_time++;
}

END_OF_STATIC_FUNCTION (net_time_incrementor);


int net_main_init ()
{
    LOCK_VARIABLE (net_time);
    LOCK_FUNCTION (net_time_inc);
    if (install_int_ex (net_time_incrementor, BPS_TO_TIMER (TIME_RATE)) < 0)
	return -1;

    net_init ();
    net_loadconfig (0);
    net_drivers = net_getdrivernames (net_initdrivers (net_drivers_all));

    return 0;
}


void net_main_shutdown ()
{
    free (net_drivers);
    net_shutdown ();
    remove_int (net_time_incrementor);
}
