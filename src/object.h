#ifndef __included_object_h
#define __included_object_h


#include <allegro.h>
#include "objtypes.h"


typedef struct object_layer_list object_layer_list_t;


#define OBJECT_RENDER_MODE_BITMAP	0
#define OBJECT_RENDER_MODE_LAYERED	1


typedef struct object {
    object_type_t *type;
    int self;			/* Lua table ref */

    /* values we store in C, for speed reasons  */
    int x, y;

    /* rendering */
    int render;
    BITMAP *bitmap;		/* bitmap render mode */
    object_layer_list_t *layer;	/* layered render mode */

    struct object *prev, *next;
} object_t;



#define OBJECT_LAYER_NO_ANGLE	0xffffffff


typedef struct object_layer {
    BITMAP *bitmap;
    int offsetx;
    int offsety;
    fixed angle;
} object_layer_t;


struct object_layer_list {
    int num;
    object_layer_t **layer;
};


object_layer_list_t *object_layer_list_create ();
void object_layer_list_destroy (object_layer_list_t *p);
object_layer_t *object_layer_list_add (object_layer_list_t *list, BITMAP *bmp, int offsetx, int offsety, fixed angle);


#endif
