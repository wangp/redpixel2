/* extdata.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "bitmaskg.h"
#include "extdata.h"
#include "magic4x4.h"


#define DAT_MAGK	DAT_ID ('M','A','G','K')
#define DAT_MASK	DAT_ID ('M','A','S','K')


static int property_exists (DATAFILE *d, int prop)
{
    return ustrcmp (get_datafile_property (d, prop), empty_string) != 0;
}


static void callback (DATAFILE *d)
{
    if (d->type == DAT_BITMAP) {
	if (property_exists (d, DAT_MASK)) {
	    BITMAP *old = d->dat;
	    d->dat = bitmask_create_from_bitmap (old);
	    d->type = DAT_MASK;
	    destroy_bitmap (old);
	}
	else if (!property_exists (d, DAT_MAGK)) {
	    BITMAP *old = d->dat;
	    d->dat = get_magic_bitmap_format (old, 0);
	    destroy_bitmap (old);
	}
    }
}


DATAFILE *load_extended_datafile (const char *filename)
{
    return load_datafile_callback (filename, callback);
}
