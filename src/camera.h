#ifndef __included_camera_h
#define __included_camera_h


#include "object.h"


typedef struct camera {
    float x, y;
    float xv, yv;
    int target_x;
    int target_y;
    int view_width;
    int view_height;
} camera_t;


void camera_track_object (camera_t *cam, object_t *obj);
void camera_track_object_with_mouse (camera_t *cam, object_t *obj, 
				     int mouse_x, int mouse_y, int max_dist);

#endif
