/* magicld.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "magicld.h"
#include "magic4x4.h"


static void magicify (DATAFILE *d)
{
    if (d->type == DAT_BITMAP) {
	BITMAP *old = d->dat;
	d->dat = get_magic_bitmap_format (old, 0);
	destroy_bitmap (old);
    }    
}


DATAFILE *load_magic_datafile (const char *filename)
{
    return load_datafile_callback (filename, magicify);
}
