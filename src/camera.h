#ifndef __included_camera_h
#define __included_camera_h


#include "objmin.h"


typedef struct camera camera_t;


camera_t *camera_create (int view_width, int view_height);
void camera_destroy (camera_t *cam);
float camera_x (camera_t *cam);
float camera_y (camera_t *cam);
int camera_view_width (camera_t *cam);
int camera_view_height (camera_t *cam);
void camera_track_object (camera_t *cam, object_t *obj);
void camera_track_object_with_mouse (camera_t *cam, object_t *obj, 
				     int mouse_x, int mouse_y, int max_dist);


#endif
