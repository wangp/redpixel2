#ifndef _included_rpx_h_
#define _included_rpx_h_

#include "defs.h"
#include "tiles.h"


typedef struct rpx
{
    int w, h;    
    tile_t tile[RPX_MAX_HEIGHT][RPX_MAX_WIDTH];  
} rpx_t;


extern rpx_t rpx;


void reset_rpx();


#endif
