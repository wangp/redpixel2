/* rpx.c : stuff to do with .RPX files
 */

#include "rpx.h"


rpx_t rpx;


/* reset_rpx:
 *  Reset RPX to default values.
 */
void reset_rpx()
{
    int x, y;
    
    rpx.w = 64;
    rpx.h = 64;
    
    for (y=0; y<RPX_MAX_HEIGHT; y++) {
	for (x=0; x<RPX_MAX_WIDTH; x++) {
	    rpx.tile[y][x] = TILE_BLANK;
	}
    }
}


/* load_rpx:
 *  Loads an RPX file, returning zero on error.
 */
int load_rpx(char *filename)
{
    return 0;
}


/* save_rpx:
 *  Write an RPX file to disk, returning zero on error.
 */
int save_rpx(char *filename)
{
    return 0;
}

