#ifndef __included_objlayer_h
#define __included_objlayer_h


#include <allegro.h>


typedef struct objlayer {
    struct objlayer *next;
    struct objlayer *prev;
    int id;
    BITMAP *bmp;
    int offset_x;
    int offset_y;
} objlayer_t;


#endif
