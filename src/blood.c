/* blood.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


/*
  XXX -- use linked lists?  would be faster
 */


#include <math.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>
#include "alloc.h"
#include "bitmask.h"
#include "blood.h"
#include "map.h"


#define INITIAL_NUMBER_OF_PARTICLES	10000


typedef struct {
    float x, y;
    float xv, yv;
    int color;
    int life;
} particle_t;


struct blood_particles {
    int nparticles;
    particle_t *particles;
};


blood_particles_t *blood_particles_create (void)
{
    blood_particles_t *blood = alloc (sizeof *blood);

    blood->nparticles = INITIAL_NUMBER_OF_PARTICLES;
    blood->particles = alloc (INITIAL_NUMBER_OF_PARTICLES * sizeof (particle_t));

    return blood;
}


void blood_particles_destroy (blood_particles_t *p)
{
    if (p) {
	free (p->particles);
	free (p);
    }
}


void blood_particles_update (blood_particles_t *blood, map_t *map)
{
    bitmask_t *mask = map_tile_mask (map);
    particle_t *p = blood->particles;
    int n = blood->nparticles;

    while (n--) {
	if (p->life) {
	    p->x += p->xv;
	    p->y += p->yv;
	    p->xv *= 0.99;
	    p->yv += 0.05;
	    if (bitmask_point (mask, p->x, p->y))
		p->life = 0;
	    else
		p->life--;
	}
	p++;
    }
}
 

void blood_particles_spawn (blood_particles_t *blood, float x, float y, long nparticles, long spread)
{
    particle_t *p = blood->particles;
    int n = blood->nparticles;
    double theta;
	
    while (n-- && nparticles) {
	if (!p->life) {
	    theta = (rand() % (int)(M_PI * 2. * 1000.)) / 1000.;
	    p->x = x + (rand()%7) - 6;
	    p->y = y + (rand()%7) - 6;
	    p->xv = (rand() % spread) * cos (theta) / 1000.;
	    p->yv = (rand() % spread) * sin (theta) / 1000.;
	    p->life = 100;
	    p->color = makecol24 (4 + (rand() % 8), 0, 0);

	    nparticles--;
	}

	p++;
    }
}


void blood_particles_draw (BITMAP *bmp, blood_particles_t *blood, int offset_x, int offset_y)
{
    int old_cl = bmp->cl;
    int old_cr = bmp->cr;
    bmp->cl /= 3;
    bmp->cr /= 3;

    {
	particle_t *p = blood->particles;
	int n = blood->nparticles;
	while (n--) {
	    if (p->life)
		_linear_putpixel24 (bmp, p->x - offset_x, p->y - offset_y, p->color);
	    p++;
	}
    }

    bmp->cl = old_cl;
    bmp->cr = old_cr;
}
