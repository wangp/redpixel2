#ifndef __included_object_h
#define __included_object_h


#include <allegro.h>
#include "bitmask.h"
#include "objtypes.h"


typedef struct object_image object_image_t;
typedef struct object_anim object_anim_t;


#define OBJECT_ROLE_PROXY		1
#define OBJECT_ROLE_AUTHORITY		2

#define OBJECT_RENDER_MODE_BITMAP	0
#define OBJECT_RENDER_MODE_IMAGE	1
#define OBJECT_RENDER_MODE_ANIM		2


typedef struct object {
    object_type_t *type;
    unsigned long id;
    int self;			/* Lua table ref */
    int role;

    /* values we store in C, for speed reasons  */
    float x, y;
    float xv, yv;

    float old_x, old_y;
    float old_xv, old_yv;

    /* collision masks */
    bitmask_t *mask;
    bitmask_t *mask_top, *mask_bottom;	/* optional */
    bitmask_t *mask_left, *mask_right;	/* optional */

    /* rendering */
    int render;
    BITMAP *bitmap;		/* bitmap render mode */
    object_image_t *image;	/* image render mode */
    object_anim_t *anim;	/* animation render mode */

    int dying;
    struct object *prev, *next;
} object_t;


object_t *object_create (const char *type_name, int role);
void object_destroy (object_t *p);
void object_set_render_mode (object_t *p, int mode, void *data);
void object_free_render_data (object_t *p);


/*
 *  Layered render mode (image)
 */

#define OBJECT_LAYER_NO_ANGLE	0xffffffff


typedef struct object_layer {
    BITMAP *bitmap;
    int offsetx;
    int offsety;
    fixed angle;
} object_layer_t;

struct object_image {
    int num;
    object_layer_t **layer;
};


object_image_t *object_image_create ();
void object_image_destroy (object_image_t *p);
object_layer_t *object_image_add_layer (object_image_t *image, BITMAP *bmp,
					int offsetx, int offsety, fixed angle);


/*
 *  Animation render mode
 */

struct object_anim {
    int num;
    BITMAP **bitmap;
    int *tic;

    int current, ticker;
};


object_anim_t *object_anim_create ();
void object_anim_destroy (object_anim_t *p);
int object_anim_add_frame (object_anim_t *anim, BITMAP *bmp, int tics);
void object_anim_advance (object_anim_t *p);


#endif
