#ifndef _included_rpx_h_
#define _included_rpx_h_

#include "defs.h"
#include "tiles.h"
#include "object.h"


/* from Allegro.h */
#define RPX_ID(d,c,b,a)		((a<<24) | (b<<16) | (c<<8) | d)

#define RPX_MAGIC	RPX_ID('.','r','p','x')
#define RPX_NAME	RPX_ID('N','A','M','E')
#define RPX_SIZE	RPX_ID('S','I','Z','E')
#define RPX_TILETBL	RPX_ID('T','T','B','L')
#define RPX_TILEDAT	RPX_ID('T','I','L','E')
#define RPX_OBJS	RPX_ID('O','B','J','S')


typedef struct rpx
{
    char name[256];
    
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
