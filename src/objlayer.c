/* objlayer.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "object.h"


object_image_t *object_image_create ()
{
    return alloc (sizeof (object_image_t));
}


void object_image_destroy (object_image_t *p)
{
    int i;

    for (i = 0; i < p->num; i++)
	free (p->layer[i]);
    free (p);
}


object_layer_t *object_image_add_layer (object_image_t *image, BITMAP *bmp, int offsetx, int offsety, fixed angle)
{
    object_layer_t *layer;
    void *p;
    
    layer = alloc (sizeof *layer);
    if (!layer) return 0;
    
    p = realloc (image->layer, sizeof (object_layer_t *) * (image->num + 1));
    if (!p) {
	free (layer);
	return 0;
    }
    else
	image->layer = p;

    layer->bitmap  = bmp;
    layer->offsetx = offsetx;
    layer->offsety = offsety;
    layer->angle   = angle;
    
    image->layer[image->num] = layer;
    image->num++;

    return layer;
}
