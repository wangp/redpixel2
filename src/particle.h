#ifndef __included_particle_h
#define __included_particle_h


struct map;
struct BITMAP;


typedef struct particles particles_t;


particles_t *particles_create (void);
void particles_destroy (particles_t *);
void particles_update (particles_t *, struct map *);
void particles_spawn_blood (particles_t *, float x, float y, long nparticles, float spread);
void particles_spawn_spark (particles_t *, float x, float y, long nparticles, float spread);
void particles_spawn_respawn_particles (particles_t *, float x, float y, long nparticles, float spread);
void particles_draw (struct BITMAP *, particles_t *, int offset_x, int offset_y);


#endif
