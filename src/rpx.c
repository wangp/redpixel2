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



/*
 * rpx-object related : mostly for map editor 
 */


/* rpx_link_object:
 *  Add an object to the object l'list.
 */
void rpx_link_object(object_t *obj)
{    
    /* for speed we just stick it up the front */
    if (rpx.obj_head) {
	rpx.obj_head->prev = obj;
	obj->next = rpx.obj_head;
	obj->prev = NULL;
	rpx.obj_head = obj;
    } 
    else {
	rpx.obj_head = obj;
	obj->next = obj->prev = NULL;
    }    
}


/* rpx_unlink_object:
 *  Delete OBJ from the list.
 *  Does NOT free the memory.
 */
void rpx_unlink_object(object_t *obj)
{
    if (obj == rpx.obj_head) {
	rpx.obj_head->next = obj->next;
	return;
    }
    
    if (obj->prev)
      obj->prev->next = obj->next;
    
    if (obj->next)
      obj->next->prev = obj->prev;
}


