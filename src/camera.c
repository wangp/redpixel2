/* camera.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "camera.h"
#include "fastsqrt.h"
#include "object.h"


static void move_camera_closer_to_target (camera_t *cam)
{
    float dx, dy;

    dx = cam->target_x - cam->x;
    dy = cam->target_y - cam->y;
    
    cam->xv += dx * 0.17;	/* [0.1] */
    cam->yv += dy * 0.17;	/* [0.1] */

    cam->x += cam->xv;
    cam->y += cam->yv;

    cam->xv *= 0.35;		/* [0.9] */
    cam->yv *= 0.35;		/* [0.9] */

    /* For a fun but effect, replace the dampening values with the
     * ones in [brackets].  */
}


void camera_track_object (camera_t *cam, object_t *obj)
{
    cam->target_x = obj->cvar.x - (cam->view_width / 2);
    cam->target_y = obj->cvar.y - (cam->view_height / 2);

    move_camera_closer_to_target (cam);
}


/* Camera tracks the point between an object and the mouse position.  */
void camera_track_object_with_mouse (camera_t *cam, object_t *obj,
				     int mouse_x, int mouse_y, int max_dist)
{
    int cx, cy;
    int dx, dy;

    cx = cam->view_width / 2;
    cy = cam->view_height / 2;
    dx = mouse_x - cx;
    dy = mouse_y - cy;

    if (fast_fsqrt ((dx * dx) + (dy * dy)) > max_dist) {
	float angle = atan2 (dy, dx);
	dx = max_dist * cos (angle);
	dy = max_dist * sin (angle);
    }

    cam->target_x = (obj->cvar.x + cam->x - cx + dx) / 2;
    cam->target_y = (obj->cvar.y + cam->y - cy + dy) / 2;

    move_camera_closer_to_target (cam);
}
