/* error.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include <allegro.h>
#include "error.h"


void error (const char *message)
{
    allegro_exit ();
    allegro_message (message);
    exit (1);
}


void errorv (const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    va_end (fmt);

    error (buf);
}
