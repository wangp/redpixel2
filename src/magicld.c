/* magicld.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "magicld.h"
#include "magic4x4.h"


#define DAT_MAGK	DAT_ID ('M','A','G','K')


static void magicify (DATAFILE *d)
{
    if ((d->type == DAT_BITMAP)
	&& (!ustrcmp (get_datafile_property (d, DAT_MAGK), empty_string))) {
	BITMAP *old = d->dat;
	d->dat = get_magic_bitmap_format (old, 0);
	destroy_bitmap (old);
    }    
}


DATAFILE *load_magic_datafile (const char *filename)
{
    return load_datafile_callback (filename, magicify);
}
