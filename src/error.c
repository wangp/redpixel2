/* error.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "error.h"


void error (const char *message)
{
    allegro_exit ();
    allegro_message (message);
    exit (1);
}

