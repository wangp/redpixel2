#ifndef __included_objlight_h
#define __included_objlight_h


#include <allegro.h>


typedef struct objlight {
    struct objlight *next;
    struct objlight *prev;
    int id;
    BITMAP *bmp;
    int offset_x;
    int offset_y;    
} objlight_t;


#endif
