/* clsvface.c - client-server interface
 *
 * The link is like so:
 * client <--> text messages <--> YOU ARE HERE <--> server
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "clsvface.h"
#include "messages.h"
#include "server.h"
#include "svintern.h"
#include "strlcpy.h"


static char the_line[1024];
static char have_line;


static void clsvface_init (void)
{
    have_line = 0;
}


static void clsvface_shutdown (void)
{
}


static void clsvface_add_log (const char *prefix, const char *text)
{
    if (prefix)
	messages_add ("%s: %s", prefix, text);
    else
	messages_add ("%s", text);
}


static void clsvface_set_status (const char *text)
{
    /* nothing */
}


static const char *clsvface_poll (void)
{
    if (!have_line)
	return 0;
    else {
	have_line = 0;
	return the_line;
    }
}


void client_server_interface_add_input (const char *line)
{
    strlcpy (the_line, line, sizeof the_line);
    have_line = 1;
}


static server_interface_t the_interface = {
    clsvface_init,
    clsvface_shutdown,
    clsvface_add_log,
    clsvface_set_status,
    clsvface_poll
};

server_interface_t *client_server_interface = &the_interface;
