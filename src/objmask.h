#ifndef __included_objmask_h
#define __included_objmask_h


#include "bitmask.h"


struct objmask {
    bitmask_t *mask;
    int offset_x;
    int offset_y;
};


#endif
