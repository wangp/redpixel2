#ifndef _included_rpx_h_
#define _included_rpx_h_

#include "defs.h"
#include "tiles.h"
#include "object.h"


typedef struct rpx
{
    int w, h;    
    
    tile_t tile[RPX_MAX_HEIGHT][RPX_MAX_WIDTH];
    
    struct object *obj_head;
    
} rpx_t;


extern rpx_t rpx;


void reset_rpx();
int load_rpx(char *filename);
int save_rpx(char *filename);


void rpx_link_object(object_t *obj);
void rpx_unlink_object(object_t *obj);


#endif
