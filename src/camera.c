/* camera.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include "alloc.h"
#include "camera.h"
#include "fastsqrt.h"
#include "object.h"


struct camera {
    float x, y;
    float xv, yv;
    int target_x;
    int target_y;
    int view_width;
    int view_height;
};


camera_t *camera_create (int view_width, int view_height)
{
    camera_t *cam = alloc (sizeof *cam);
    cam->view_width = view_width;
    cam->view_height = view_height;
    return cam;
}


void camera_destroy (camera_t *cam)
{
    free (cam);
}


float camera_x (camera_t *cam)
{
    return cam->x;
}


float camera_y (camera_t *cam)
{
    return cam->y;
}


int camera_view_width (camera_t *cam)
{
    return cam->view_width;
}


int camera_view_height (camera_t *cam)
{
    return cam->view_height;
}


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
    cam->target_x = object_x (obj) - (cam->view_width / 2);
    cam->target_y = object_y (obj) - (cam->view_height / 2);

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

    cam->target_x = (object_x (obj) + cam->x - cx + dx) / 2;
    cam->target_y = (object_y (obj) + cam->y - cy + dy) / 2;

    move_camera_closer_to_target (cam);
}
