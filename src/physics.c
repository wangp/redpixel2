/* physics.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


/* XXX most or all of this stuff should move into object.c one day, or
   movement stuff from object.c moved out */


#include <stdlib.h>
#include "alloc.h"
#include "map.h"
#include "object.h"
#include "physics.h"


#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))

static inline float clamp (float n)
{
    return (ABS (n) < 0.001) ? 0 : n;   /* XXX find good clamp value */
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
    float xv = object_xv (obj) + object_xa (obj);

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
    float yv = object_yv (obj) + object_mass (obj);

    if (object_move (obj, ((yv < 0) ? OBJECT_MASK_TOP : OBJECT_MASK_BOTTOM), phys->map, 0, yv) < 0) {
	/* object stopped short of an entire yv */
	object_set_ya (obj, 0);
	object_set_yv (obj, 0);
	object_set_jump (obj, 0);
    }
    else {
	/* object moved entire yv */
	object_set_ya (obj, object_mass (obj));
	object_set_yv (obj, clamp (yv * object_yv_decay (obj)));
    }
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
    float xa = object_xa (obj);
    float ya = object_ya (obj);

    move_object_x (phys, obj);
    if (object_stale (obj)) return;

    move_object_y (phys, obj);
    if (object_stale (obj)) return;

    if ((xa != object_xa (obj)) || (ya != object_ya (obj)))
	object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE);

    if (!object_is_client (obj) && (too_far_off_map (obj, phys->map)))
	object_set_stale (obj);
}


/* Game client physics. */


#if 0
void physics_extrapolate_proxy (physics_t *phys, object_t *obj)
{
    float xv = object_xv (obj);
    float yv = object_yv (obj);

    object_set_xy (obj, object_x (obj) /*+ xv*/, object_y (obj) + yv);
    object_set_xvyv (obj, clamp (xv * object_xv_decay (obj)), clamp (yv * object_yv_decay (obj)));
}
#endif


void physics_extrapolate_proxy (physics_t *phys, object_t *obj)
{
    float xv = object_xv (obj) + object_xa (obj);
    float yv = object_yv (obj) + object_ya (obj);

    object_set_xy (obj, object_x (obj) + xv, object_y (obj) + yv);
    object_set_xv (obj, clamp (xv * object_xv_decay (obj)));
    object_set_yv (obj, clamp (yv * object_yv_decay (obj)));
}
