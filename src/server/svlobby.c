/* svlobby.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "netmsg.h"
#include "server.h"
#include "svclient.h"
#include "svintern.h"


static int svlobby_init (void)
{
    /*	Let's all go to the lobby,
	Let's all go to the lobby,
	Let's all go to the lobbiie,
	Get ourselves some snacks.   */

    server_log ("Entering lobby");
    svclients_broadcast_rdm_byte (MSG_SC_LOBBY);
    return 0;
}


static void svlobby_poll (void)
{
}


static void svlobby_shutdown (void)
{
    server_log ("Leaving lobby");
}


static server_state_procs_t the_procs =
{
    svlobby_init,
    svlobby_poll,
    svlobby_shutdown
};

server_state_procs_t *svlobby_procs = &the_procs;

