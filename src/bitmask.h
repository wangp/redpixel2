#ifndef __included_bitmask_h
#define __included_bitmask_h


typedef struct bitmask bitmask_t;


bitmask_t *bitmask_create (int width, int height);
void bitmask_destroy (bitmask_t *p);
int bitmask_width (bitmask_t *p);
int bitmask_height (bitmask_t *p);
void bitmask_set_point (bitmask_t *p, int x, int y, int value);
int bitmask_point (bitmask_t *p, int x, int y);
int bitmask_check_collision (bitmask_t *mask1, bitmask_t *mask2,
			     int x1, int y1, int x2, int y2);


#endif
