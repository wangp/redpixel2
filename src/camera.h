#ifndef __included_camera_h
#define __included_camera_h


#include "object.h"


typedef struct camera {
    int x, y;
    int view_width;
    int view_height;
} camera_t;


void camera_track_object (camera_t *cam, object_t *obj);


#endif
