#ifndef __included_bitmaskg_h
#define __included_bitmaskg_h


#include <allegro.h>
#include "bitmask.h"


bitmask_t *bitmask_create_from_bitmap (BITMAP *bmp);
bitmask_t *bitmask_create_from_magic_bitmap (BITMAP *bmp);


#endif
