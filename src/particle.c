/* blood.c
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
#include "blood.h"
#include "map.h"


#define INITIAL_PARTICLES	1024
#define MAX_PARTICLES		8192


typedef struct particle {
    struct particle *next;
    float x, y;
    float xv, yv;
    int color;
    int life;
} particle_t;


struct blood_particles {
    int total_particles;
    particle_t *live_particles;
    particle_t *free_particles;
};


static void alloc_free_particles (blood_particles_t *blood, int num)
{
    blood->total_particles += num;    
    while (num--) {
	particle_t *p = alloc (sizeof *p);
	p->next = blood->free_particles;
	blood->free_particles = p;
    }
}


blood_particles_t *blood_particles_create (void)
{
    blood_particles_t *blood = alloc (sizeof *blood);
    alloc_free_particles (blood, INITIAL_PARTICLES);
    return blood;
}


static void free_particles_list (particle_t *p)
{
    while (p) {
	particle_t *q = p->next;
	free (p);
	p = q;
    }
}


void blood_particles_destroy (blood_particles_t *p)
{
    if (p) {
	free_particles_list (p->free_particles);
	free_particles_list (p->live_particles);
	free (p);
    }
}


void blood_particles_update (blood_particles_t *blood, map_t *map)
{
    bitmask_t *mask = map_tile_mask (map);
    particle_t *p = blood->live_particles;
    particle_t *prev = NULL;
    particle_t *next;
    
    while (p) {
	p->x += p->xv;
	p->y += p->yv;
	p->xv *= 0.99;
	p->yv += 0.05;
	
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
		blood->live_particles = next;
	    p->next = blood->free_particles;
	    blood->free_particles = p;
	    p = next;
	}
    }
}
 

void blood_particles_spawn (blood_particles_t *blood, float x, float y, long nparticles, long spread)
{
    particle_t *p;
    double theta;
	
    while (nparticles--) {

	/* if out of free particles allocate some more or abort */
	if (!blood->free_particles) {
	    int n;

	    if (blood->total_particles >= MAX_PARTICLES)
		break;

	    if (nparticles > MAX_PARTICLES - blood->total_particles)
		n = MAX_PARTICLES - blood->total_particles;
	    else
		n = nparticles;	    
	    alloc_free_particles (blood, n);
	}

	p = blood->free_particles;
	blood->free_particles = p->next;

	theta = (rand() % (int)(M_PI * 2. * 1000.)) / 1000.;
	p->x = x + (rand()%7) - 6;
	p->y = y + (rand()%7) - 6;
	p->xv = (rand() % spread) * cos (theta) / 1000.;
	p->yv = (rand() % spread) * sin (theta) / 1000.;
	p->life = 100;
	p->color = makecol24 (4 + (rand() % 8), 0, 0);

	p->next = blood->live_particles;
	blood->live_particles = p;
    }
}


void blood_particles_draw (BITMAP *bmp, blood_particles_t *blood, int offset_x, int offset_y)
{
    int old_cl = bmp->cl;
    int old_cr = bmp->cr;
    bmp->cl /= 3;
    bmp->cr /= 3;

    {
	particle_t *p;
	for (p = blood->live_particles; p; p = p->next)
	    _linear_putpixel24 (bmp, p->x - offset_x, p->y - offset_y, p->color);
    }

    bmp->cl = old_cl;
    bmp->cr = old_cr;
}
