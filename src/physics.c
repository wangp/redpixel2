/* physics.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include "alloc.h"
#include "map.h"
#include "object.h"
#include "physics.h"


#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))

static inline float clamp (float n)
{
    return (ABS (n) < 0.25) ? 0 : n;
}


struct physics {
    map_t *map;
    float x_decay;
    float y_decay;
};


physics_t *physics_create (map_t *map)
{
    physics_t *phys = alloc (sizeof *phys);

    if (phys) {
	phys->map = map;
	phys->x_decay = 0.75;
	phys->y_decay = 0.75;
    }

    return phys;
}


void physics_destroy (physics_t *phys)
{
    free (phys);
}


/* Game server physics. */


/* XXX move elsewhere */
#define MASK_MAIN	0
#define MASK_TOP	1
#define MASK_BOTTOM	2
#define MASK_LEFT	3
#define MASK_RIGHT	4


static void move_object_x (physics_t *phys, object_t *obj)
{
    float xv;

    if (!(xv = object_xv (obj)))
	return;

    if (object_move_x_with_ramp (obj, ((xv < 0) ? MASK_LEFT : MASK_RIGHT),
				 phys->map, xv, object_ramp (obj)) < 0) {
	/* object stopped short of an entire xv */
	object_set_xv (obj, 0);
	object_set_need_replication (obj);
    }
    else {
	/* object moved entire xv */
	object_set_xv (obj, clamp (xv * phys->x_decay));
    }
}


static void move_object_y (physics_t *phys, object_t *obj)
{
    float yv, old_client_yv;

    /* how much we want to move */
    if (!(yv = object_yv (obj) + object_mass (obj)))
	return;

    old_client_yv = object_client_yv (obj);

    if (object_move (obj, ((yv < 0) ? MASK_TOP : MASK_BOTTOM), phys->map, 0, yv) < 0) {
	/* object stopped short of an entire yv */
	object_set_yv (obj, 0);
	object_set_jump (obj, 0);
	object_set_client_yv (obj, 0);
    }
    else {
	/* object moved entire yv */
	yv = clamp (yv * phys->y_decay);
	object_set_yv (obj, yv);
	object_set_client_yv (obj, yv + object_mass (obj));
    }

    if (old_client_yv != object_client_yv (obj))
	object_set_need_replication (obj);
}


void physics_move_object (physics_t *phys, object_t *obj)
{
    move_object_x (phys, obj);
    move_object_y (phys, obj);
}


/* Game client physics. */


void physics_interpolate_object (physics_t *phys, object_t *obj, int nticks)
{
    float x = object_real_x (obj);
    float y = object_real_y (obj);
    float xv = object_xv (obj);
    float yv = object_yv (obj);

    while ((nticks--) && ((xv) || (yv))) {
	x += xv;
	y += yv;
	xv = clamp (xv * phys->x_decay);
	yv = clamp (yv * phys->y_decay);
    }

    object_set_xy (obj, x, y);
}
