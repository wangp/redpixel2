#ifndef __included_bitmaskg_h
#define __included_bitmaskg_h


struct BITMAP;
struct bitmask;


struct bitmask *bitmask_create_from_bitmap (struct BITMAP *bmp);
struct bitmask *bitmask_create_from_magic_bitmap (struct BITMAP *bmp);


#endif
