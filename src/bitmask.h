#ifndef __included_bitmask_h
#define __included_bitmask_h


typedef struct bitmask bitmask_t;


/* Internal:
 */

/* Set this to a 32 bit unsigned integer on your machine, or
 * alternatively, check the code works with other sizes.  */
typedef uint32_t _bitmask_u32_t;

#define _bitmask_u32_bytes	(sizeof (_bitmask_u32_t))
#define _bitmask_u32_bits	(_bitmask_u32_bytes * 8)

struct bitmask {	/* internal */
    int w, h;
    int line_w;		/* width of a line[] in bytes */
    void *dat;
    _bitmask_u32_t *line[0];
};

/* End internal.
 */


bitmask_t *bitmask_create (int width, int height);
void bitmask_destroy (bitmask_t *p);
int bitmask_width (bitmask_t *p);
int bitmask_height (bitmask_t *p);
void bitmask_set_point (bitmask_t *p, int x, int y, int value);

static inline int bitmask_point (bitmask_t *p, int x, int y)
{
    if ((x < 0) || (y < 0) || (x >= p->w) || (y >= p->h))
	return 0;
    else {    
	int u = x / _bitmask_u32_bits;
	return p->line[y][u] & (1 << ((_bitmask_u32_bits - 1) - (x - (u * _bitmask_u32_bits))));
    }
}

int bitmask_check_collision (bitmask_t *mask1, bitmask_t *mask2,
			     int x1, int y1, int x2, int y2);


#endif
