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


struct physics {
    map_t *map;
    float gravity;
    float x_decay;
    float y_decay;
};


static inline float clamp (float n)
{
    return (ABS (n) < 0.25) ? 0 : n;
}


static void move_object_x (physics_t *phys, object_t *obj)
{
    float xv = object_xv (obj);

    if (object_move_x_with_ramp (obj, ((xv < 0) ? 3 : 4), phys->map, xv, object_ramp (obj)) < 0) {
	object_set_xv (obj, 0);
	return;
    }

    object_set_xv (obj, clamp (xv * phys->x_decay));
}


static void move_object_y (physics_t *phys, object_t *obj)
{
    float yv = object_yv (obj) + (object_mass (obj) * phys->gravity);

    if (object_move (obj, ((yv < 0) ? 1 : 2), phys->map, 0, yv) < 0) {
	object_set_yv (obj, 0);
	object_set_jump (obj, 0);
	return;
    }

    object_set_yv (obj, clamp (yv * phys->y_decay));
}


int physics_move_object (physics_t *phys, object_t *obj)
{
    float oldx = object_x (obj);
    float oldy = object_y (obj);

    move_object_x (phys, obj);
    move_object_y (phys, obj);

    return (oldx != object_x (obj)) || (oldy != object_y (obj));
}


physics_t *physics_create (map_t *map)
{
    physics_t *phys = alloc (sizeof *phys);

    if (phys) {
	phys->map = map;
	phys->gravity = 1.0;
	phys->x_decay = 0.75;
	phys->y_decay = 0.75;
    }

    return phys;
}


void physics_destroy (physics_t *phys)
{
    free (phys);
}
