#ifndef __included_physics_h
#define __included_physics_h


typedef struct physics physics_t;


physics_t *physics_create (map_t *);
void physics_destroy (physics_t *);
int physics_move_object (physics_t *, object_t *);


#endif
