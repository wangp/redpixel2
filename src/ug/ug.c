/* ug.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "ug.h"
#include "ugtheme.h"


static int count;


int ug_init ()
{
    if (!count) {
	if (ug_theme_init () < 0)
	    return -1;
    }

    count++;
    return 0;
}


void ug_shutdown ()
{
    if (--count < 1) 
	ug_theme_shutdown ();
}
