/* explo.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "explo.h"
#include "list.h"
#include "magic4x4.h"
#include "sound.h"
#include "store.h"


typedef struct explosion_type etype_t;

struct explosion_type {
    etype_t *next;
    etype_t *prev;
    char *name;
    DATAFILE *first_frame;
    int nframes;
    int tics;
    int centre_x;
    int centre_y;
    BITMAP *light;
    SAMPLE *sound;
};


struct explosion {
    explosion_t *next;
    explosion_t *prev;
    etype_t *type;
    int x, y;
    int frame;
    int tics;
};


static list_head_t etypes;


void explosion_init (void)
{
    list_init (etypes);
}


static void free_etype (etype_t *t)
{
    free (t->name);
    free (t);
}


void explosion_shutdown (void)
{
    list_free (etypes, free_etype);
}


int explosion_type_register (const char *name, const char *first_frame,
			     int nframes, int tics, const char *light,
			     const char *sound)
{
    DATAFILE *d;
    etype_t *t;

    d = store_datafile (first_frame);
    if (!d)
	return -1;

    t = alloc (sizeof *t);
    t->name = ustrdup (name);
    t->first_frame = d;
    t->nframes = nframes;
    t->tics = tics;
    t->centre_x = ((BITMAP *) d->dat)->w/3/2;
    t->centre_y = ((BITMAP *) d->dat)->h/2;
    t->light = light ? store_dat (light) : NULL;
    t->sound = sound ? store_dat (sound) : NULL;
    
    list_add (etypes, t);
    
    return 0;
}


static etype_t *find_type_by_name (const char *name)
{
    etype_t *t;
    
    list_for_each (t, &etypes)
	if (0 == ustrcmp (t->name, name))
	    return t;
    
    return NULL;
}


explosion_t *explosion_create (const char *name, int x, int y)
{
    etype_t *t;
    explosion_t *e;

    t = find_type_by_name (name);
    if (!t)
	return NULL;

    e = alloc (sizeof *e);
    e->type = t;
    e->x = x;
    e->y = y;
    e->frame = 0;
    e->tics = t->tics;

    if (t->sound)
	sound_play_once (t->sound, 255, e->x, e->y);
    
    return e;
}


void explosion_destroy (explosion_t *e)
{
    free (e);
}


int explosion_update (explosion_t *e)
{
    if (--e->tics > 0)
	return 0;

    if (++e->frame >= e->type->nframes)
	return -1;

    e->tics = e->type->tics;
    return 0;
}


void explosion_draw (BITMAP *dest, explosion_t *explo,
		     int offset_x, int offset_y)
{
    draw_trans_magic_sprite (dest,
			     explo->type->first_frame[explo->frame].dat,
			     explo->x - offset_x - explo->type->centre_x,
			     explo->y - offset_y - explo->type->centre_y);
}


void explosion_draw_lights (BITMAP *dest, explosion_t *explo,
			    int offset_x, int offset_y)
{
    BITMAP *bmp;

    if (explo->frame >= explo->type->nframes * 0.6)
	return;

    if ((bmp = explo->type->light))
	draw_trans_magic_sprite (dest, bmp,
				 explo->x - offset_x - bmp->w/3/2,
				 explo->y - offset_y - bmp->h/2);
}
