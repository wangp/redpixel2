/* objanim.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "object.h"


object_anim_t *object_anim_create ()
{
    return alloc (sizeof (object_anim_t));
}


void object_anim_destroy (object_anim_t *p)
{
    free (p->bitmap);
    free (p->tic);
    free (p);
}


int object_anim_add_frame (object_anim_t *anim, BITMAP *bmp, int tics)
{
    void *p;

    p = realloc (anim->bitmap, sizeof (BITMAP *) * (anim->num + 1));
    if (!p) return -1;
    anim->bitmap = p;
        
    p = realloc (anim->tic, sizeof (int) * (anim->num + 1));
    if (!p) return -1;
    anim->tic = p;

    anim->bitmap[anim->num] = bmp;
    anim->tic[anim->num] = tics;
    anim->num++;

    return 0;
}
