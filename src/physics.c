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
};


physics_t *physics_create (map_t *map)
{
    physics_t *phys = alloc (sizeof *phys);

    if (phys)
	phys->map = map;

    return phys;
}


void physics_destroy (physics_t *phys)
{
    free (phys);
}


/* Game server physics. */


static void move_object_x (physics_t *phys, object_t *obj)
{
    float xv;

    if (!(xv = object_xv (obj)))
	return;

    if (object_move_x_with_ramp (obj, ((xv < 0) ? OBJECT_MASK_LEFT : OBJECT_MASK_RIGHT),
				 phys->map, xv, object_ramp (obj)) < 0) {
	/* object stopped short of an entire xv */
	object_set_xv (obj, 0);
	object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE);
    }
    else {
	/* object moved entire xv */
	object_set_xv (obj, clamp (xv * object_xv_decay (obj)));
    }
}


static void move_object_y (physics_t *phys, object_t *obj)
{
    float yv, old_proxy_yv;

    /* how much we want to move */
    if (!(yv = object_yv (obj) + object_mass (obj)))
	return;

    old_proxy_yv = object_proxy_yv (obj);

    if (object_move (obj, ((yv < 0) ? OBJECT_MASK_TOP : OBJECT_MASK_BOTTOM),
		     phys->map, 0, yv) < 0) {
	/* object stopped short of an entire yv */
	object_set_yv (obj, 0);
	object_set_jump (obj, 0);
	object_set_proxy_yv (obj, 0);
    }
    else {
	/* object moved entire yv */
	yv = clamp (yv * object_yv_decay (obj));
	object_set_yv (obj, yv);
	object_set_proxy_yv (obj, yv + object_mass (obj));
    }

    /* Like everyone says, you shouldn't compare FP values for
       equality.  You get weird stuff like 1.500 == 2.000.  I will
       have to read up to see what this hack is actually doing. */
    if ((old_proxy_yv * 10) != (object_proxy_yv (obj) * 10))
	object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE);
}


static int too_far_off_map (object_t *obj, map_t *map)
{
    const float margin = 160;
    return ((object_x (obj) < -margin) || (object_y (obj) < -margin)
	    || (object_x (obj) > (map_width (map) * 16 + margin))
	    || (object_y (obj) > (map_height (map) * 16 + margin)));
}


void physics_move_object (physics_t *phys, object_t *obj)
{
    move_object_x (phys, obj);
    move_object_y (phys, obj);

    if ((object_id (obj) >= 0) && (too_far_off_map (obj, phys->map)))
	object_set_stale (obj);
}


/* Game client physics. */


void physics_interpolate_proxy (physics_t *phys, object_t *obj)
{
    float xv = object_xv (obj);
    float yv = object_yv (obj);

    object_set_xy (obj, object_x (obj) + xv, object_y (obj) + yv);
    object_set_xvyv (obj, clamp (xv * object_xv_decay (obj)), clamp (yv * object_yv_decay (obj)));
}
