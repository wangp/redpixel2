/* particle.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


/*
  XXX -- fragmentation problems with so many small allocs?
 */


#include <math.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>
#include "alloc.h"
#include "bitmask.h"
#include "map.h"
#include "particle.h"


#ifndef M_PI
# define M_PI   3.14159265358979323846
#endif


#define INITIAL_PARTICLES	1024
#define MAX_PARTICLES		8192


typedef struct particle {
    struct particle *next;
    float x, y;
    float xv, yv;
    int color;
    short life;
    short weightless;
} particle_t;


struct particles {
    int total_particles;
    particle_t *live_particles;
    particle_t *free_particles;
};


static int alloc_free_particles (particles_t *part, int num)
{
    if (part->total_particles >= MAX_PARTICLES)
	return -1;

    if (num > MAX_PARTICLES - part->total_particles)
	num = MAX_PARTICLES - part->total_particles;

    part->total_particles += num;    
    while (num--) {
	particle_t *p = alloc (sizeof *p);
	p->next = part->free_particles;
	part->free_particles = p;
    }

    return 0;
}


particles_t *particles_create (void)
{
    particles_t *part = alloc (sizeof *part);
    alloc_free_particles (part, INITIAL_PARTICLES);
    return part;
}


static void free_particles_list (particle_t *p)
{
    while (p) {
	particle_t *q = p->next;
	free (p);
	p = q;
    }
}


void particles_destroy (particles_t *p)
{
    if (p) {
	free_particles_list (p->free_particles);
	free_particles_list (p->live_particles);
	free (p);
    }
}


void particles_update (particles_t *part, map_t *map)
{
    bitmask_t *mask = map_tile_mask (map);
    particle_t *p = part->live_particles;
    particle_t *prev = NULL;
    particle_t *next;
    
    while (p) {
	p->x += p->xv;
	p->y += p->yv;
	if (!p->weightless) {
	    p->xv *= 0.995;
	    p->yv += 0.05;
	}
	
	if (bitmask_point (mask, p->x, p->y))
	    p->life = 0;
	else
	    p->life--;

	if (p->life > 0) {
	    prev = p;
	    p = p->next;
	}
	else {
	    /* particle dead: move to free list */
	    next = p->next;
	    if (prev)
		prev->next = next;
	    else
		part->live_particles = next;
	    p->next = part->free_particles;
	    part->free_particles = p;
	    p = next;
	}
    }
}


static inline int rnd (int lower, int upper)
{
    return (rand() % (upper-lower+1)) + lower;
}
 

enum {
    TYPE_BLOOD,
    TYPE_SPARK,
    TYPE_RESPAWNING
};
 

static void particles_spawn (particles_t *part, int type,
			     float x, float y, long nparticles, float spread)
{
    particle_t *p;
    double theta;
    int r, g, b;
    float h, s, v;

    while (nparticles > 0) {
	/* if out of free particles allocate some more or abort */
	if ((!part->free_particles) &&
	    (alloc_free_particles (part, nparticles) < 0))
	    break;

	/* get free particle */
	p = part->free_particles;
	part->free_particles = p->next;

	/* initialise it */
	switch (type) {

	    case TYPE_BLOOD:
		theta = rnd (0, M_PI * 2 * 1000) / 1000.;
		p->x = x + rnd (-3, 3);
		p->y = y + rnd (-3, 3);
		p->xv = rnd (0, spread * 1000) * cos (theta) / 1000.;
		p->yv = rnd (0, spread * 1000) * sin (theta) / 1000.;
		p->life = 100;
		p->weightless = 0;
		
		r = rnd (4, 9);
		g = rnd (0, 2);
		b = rnd (1, 3);
		if (!rnd (0, 32)) {
		    r += rnd (0, 3);
		    g += rnd (0, 3);
		    b += rnd (0, 3);
		}
		p->color = makecol24 (r, g, b);
		break;

	    case TYPE_SPARK:
		theta = rnd (0, M_PI * 2 * 1000) / 1000.;
		p->x = x + rnd (-3, 3);
		p->y = y + rnd (-3, 3);
		p->xv = rnd (0, spread * 1000) * cos (theta) / 1000.;
		p->yv = rnd (0, spread * 1000) * sin (theta) / 1000.;
		p->life = 100;
		p->weightless = 0;
		
		h = rnd (30, 60);
		s = rnd (80, 100) / 100.;
		v = 0.8;
		hsv_to_rgb (h, s, v, &r, &g, &b);
		p->color = makecol24 (r/16, g/16, b/16);
		break;

	    case TYPE_RESPAWNING:
		theta = rnd (0, M_PI * 2 * 1000) / 1000.;
		p->x = x + rnd (-16, 16);
		p->y = y + rnd (-16, 16);
		p->xv = rnd (0, spread * 1000) * cos (theta) / 1000.;
		p->yv = rnd (0, spread * 1000) * sin (theta) / 1000.;
		p->life = rnd (10, 40);
		p->weightless = 1;
		
		h = rnd (180, 240);
		s = rnd (80, 100) / 100.;
		v = 0.8;
		hsv_to_rgb (h, s, v, &r, &g, &b);
		p->color = makecol24 (r/16, g/16, b/16);
		break;
	}

	/* put it into live particles list */
	p->next = part->live_particles;
	part->live_particles = p;

	nparticles--;
    }
}


void particles_spawn_blood (particles_t *part, float x, float y, long nparticles, float spread)
{
    particles_spawn (part, TYPE_BLOOD, x, y, nparticles, spread);
}


void particles_spawn_spark (particles_t *part, float x, float y, long nparticles, float spread)
{
    particles_spawn (part, TYPE_SPARK, x, y, nparticles, spread);
}


void particles_spawn_respawn_particles (particles_t *part, float x, float y, long nparticles, float spread)
{
    particles_spawn (part, TYPE_RESPAWNING, x, y, nparticles, spread);
}


void particles_draw (BITMAP *bmp, particles_t *part, int offset_x, int offset_y)
{
    int old_cl = bmp->cl;
    int old_cr = bmp->cr;
    bmp->cl /= 3;
    bmp->cr /= 3;

    {
	particle_t *p;
	for (p = part->live_particles; p; p = p->next)
	    _linear_putpixel24 (bmp, p->x - offset_x, p->y - offset_y, p->color);
    }

    bmp->cl = old_cl;
    bmp->cr = old_cr;
}
