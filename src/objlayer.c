/* objlayer.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "object.h"


object_layer_list_t *object_layer_list_create ()
{
    return alloc (sizeof (object_layer_list_t));
}


void object_layer_list_destroy (object_layer_list_t *p)
{
    int i;

    for (i = 0; i < p->num; i++)
	free (p->layer[i]);
    free (p);
}


object_layer_t *object_layer_list_add (object_layer_list_t *list, BITMAP *bmp, int offsetx, int offsety, fixed angle)
{
    object_layer_t *layer;
    void *p;
    
    layer = alloc (sizeof *layer);
    if (!layer) return 0;
    
    p = realloc (list->layer, sizeof (object_layer_t *) * (list->num + 1));
    if (!p) {
	free (layer);
	return 0;
    }
    else
	list->layer = p;

    layer->bitmap  = bmp;
    layer->offsetx = offsetx;
    layer->offsety = offsety;
    layer->angle   = angle;
    
    list->layer[list->num] = layer;
    list->num++;

    return layer;
}
