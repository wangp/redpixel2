#ifndef __included_vector_h
#define __included_vector_h


typedef struct vector vector_t;

vector_t *vector_create (int size, int elemsize);
void vector_destroy (vector_t *p);
vector_t *vector_resize (vector_t *p, int size);
vector_t *vector_grow (vector_t *p, int dsize);
int vector_size (vector_t *p);
void vector_set (vector_t *p, int i, void *d);
void *vector_get (vector_t *p, int i);
vector_t *vector_push (vector_t *p, void *d);


#endif
