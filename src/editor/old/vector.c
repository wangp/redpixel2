#if 0

/* vector.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


/*

  XXX convert this to template, so it becomes usable
  
 */


#include <stdlib.h>
#include <string.h>
#include "vector.h"


struct vector {
    int size, elemsize;
    void *dat;
};


static void *resize (void *p, int size, int elemsize, int oldsize)
{
    if (!size) {
	free (p);
	return 0;
    }
    
    if (!oldsize) 
	p = malloc (size * elemsize);
    else
        p = realloc (p, size * elemsize);
    
    if (p && (size > oldsize))
	memset (p + (oldsize * elemsize), 0, (size - oldsize) * elemsize);
    
    return p;
}


vector_t *vector_create (int size, int elemsize)
{
    vector_t *p = malloc (sizeof *p);
    if (!p) goto error;
    
    p->size = size;
    p->elemsize = elemsize;

    p->dat = resize (0, size, elemsize, 0);
    if (p->dat || !size)
	return p;

  error:
    free (p);
    return 0;
}


void vector_destroy (vector_t *p)
{
    if (p) {
	free (p->dat);
	free (p);
    }
}


vector_t *vector_resize (vector_t *p, int size)
{
    void *q = resize (p->dat, size, p->elemsize, p->size);

    if (q) {
	p->size = size;
	p->dat = q;
    }
    
    return (q) ? p : 0;
}


vector_t *vector_grow (vector_t *p, int dsize)
{
    return vector_resize (p, p->size + dsize);
}


int vector_size (vector_t *p)
{
    return (p) ? p->size : 0;
}


void vector_set (vector_t *p, int i, void *d)
{
    memcpy (p->dat + (i * p->elemsize), d, p->elemsize);
}


void *vector_get (vector_t *p, int i)
{
    return p->dat + (i * p->elemsize);
}


vector_t *vector_push (vector_t *p, void *d)
{
    p = vector_grow (p, 1);
    if (p)
	vector_set (p, p->size - 1, d);
    return p;
}


#endif
