/* error.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "error.h"


void error (const char *message)
{
    allegro_exit ();
    allegro_message (message);
    exit (1);
}

