/* camera.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "camera.h"
#include "object.h"


void camera_track_object (camera_t *cam, object_t *obj)
{
    cam->x = obj->cvar.x - (cam->view_width / 2);
    cam->y = obj->cvar.y - (cam->view_height / 2);
}
