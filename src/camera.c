/* camera.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include "alloc.h"
#include "camera.h"
#include "fastsqrt.h"
#include "object.h"


#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))


#define PUSH_MARGIN	15
#define STAY_MARGIN	50
#define REVERT_MARGIN	80
#define PUSH_SPEED	2.5
#define PUSH_DECAY	0.98


struct camera {
    float x, y;
    float xv, yv;
    float push_x;
    float push_y;
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
    
    cam->xv += dx * 0.11;	/* [0.1] */
    cam->yv += dy * 0.11;	/* [0.1] */

    cam->x += cam->xv;
    cam->y += cam->yv;

    cam->xv *= 0.35;		/* [0.9] */
    cam->yv *= 0.35;		/* [0.9] */

    /* For a fun effect, replace the dampening values with the
     * ones in [brackets].  */
}


void camera_track_point (camera_t *cam, int x, int y)
{
    cam->target_x = x - (cam->view_width / 2);
    cam->target_y = y - (cam->view_height / 2);

    move_camera_closer_to_target (cam);
}


void camera_track_object (camera_t *cam, object_t *obj)
{
    camera_track_point (cam, object_x (obj), object_y (obj));
}


static void handle_push (camera_t *cam, int mouse_x, int mouse_y)
{
    /* maybe this could be coded better? */
	
    /* horizontal */
    if (mouse_x < PUSH_MARGIN)
	cam->push_x -= PUSH_SPEED;
    else if (mouse_x < STAY_MARGIN)
	;
    else if (mouse_x < REVERT_MARGIN)
	cam->push_x = MIN (cam->push_x + PUSH_SPEED, 0);
    else if (mouse_x >= cam->view_width - PUSH_MARGIN)
	cam->push_x += PUSH_SPEED;
    else if (mouse_x >= cam->view_width - STAY_MARGIN)
	;
    else if (mouse_x >= cam->view_width - REVERT_MARGIN)
	cam->push_x = MAX (cam->push_x - PUSH_SPEED, 0);
    else
	cam->push_x *= PUSH_DECAY;

    /* vertical */
    if (mouse_y < PUSH_MARGIN)
	cam->push_y -= PUSH_SPEED;
    else if (mouse_y < STAY_MARGIN)
	;
    else if (mouse_y < REVERT_MARGIN)
	cam->push_y = MIN (cam->push_y + PUSH_SPEED, 0);
    else if (mouse_y >= cam->view_height - PUSH_MARGIN)
	cam->push_y += PUSH_SPEED;
    else if (mouse_y >= cam->view_height - STAY_MARGIN)
	;
    else if (mouse_y >= cam->view_height - REVERT_MARGIN)
	cam->push_y = MAX (cam->push_y - PUSH_SPEED, 0);
    else
	cam->push_y *= PUSH_DECAY;
}


/* Camera tracks the point between an object and the mouse position.  */
void camera_track_object_with_mouse (camera_t *cam, object_t *obj,
				     int mouse_x, int mouse_y, int max_dist,
				     int allow_push)
{
    int cx, cy;
    int dx, dy;

    cx = cam->view_width / 2;
    cy = cam->view_height / 2;
    dx = mouse_x - cx;
    dy = mouse_y - cy;

    if (allow_push)
	handle_push (cam, mouse_x, mouse_y);
    else {
	cam->push_x *= PUSH_DECAY;
	cam->push_y *= PUSH_DECAY;
    }

    dx += cam->push_x;
    dy += cam->push_y;

    if (fast_fsqrt ((dx * dx) + (dy * dy)) > max_dist) {
	float angle = atan2 (dy, dx);
	dx = max_dist * cos (angle);
	dy = max_dist * sin (angle);
    }

    cam->target_x = (object_x (obj) + cam->x - cx + dx) / 2;
    cam->target_y = (object_y (obj) + cam->y - cy + dy) / 2;

    move_camera_closer_to_target (cam);
}
