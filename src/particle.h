#ifndef __included_blood_h
#define __included_blood_h


struct map;
struct BITMAP;


typedef struct blood_particles blood_particles_t;


blood_particles_t *blood_particles_create (void);
void blood_particles_destroy (blood_particles_t *);
void blood_particles_update (blood_particles_t *, struct map *);
void blood_particles_spawn (blood_particles_t *, float x, float y, long nparticles, float spread);
void blood_particles_draw (struct BITMAP *, blood_particles_t *, int offset_x, int offset_y);


#endif
