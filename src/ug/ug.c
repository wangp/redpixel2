/* ug.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "ug.h"
#include "ugtheme.h"


static int count;


int ug_init (void)
{
    if (!count) {
	if (ug_theme_init () < 0)
	    return -1;
    }

    count++;
    return 0;
}


void ug_shutdown (void)
{
    if (--count < 1) 
	ug_theme_shutdown ();
}
