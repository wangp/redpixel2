/* blast.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <stdlib.h>
#include <allegro.h>
#include "alloc.h"
#include "blast.h"
#include "error.h"
#include "fastsqrt.h"
#include "list.h"
#include "mylua.h"
#include "object.h"


/* forward declaractions for helpers */
typedef struct vector vector_t;
static vector_t *create_vector (int nslots);
static void free_vector (vector_t *vec);
static void add_to_vector (vector_t *vec, void *p);
static int in_vector_p (vector_t *vec, void *p);



#define SPREAD_SPEED  4.8


struct blast {
    blast_t *next;
    blast_t *prev;
    float x;
    float y;
    float max_radius;
    int max_damage;
    float r;
    client_id_t owner;
    vector_t *already_hit;
};


/* The client should pass OBJID_CLIENT_PROCESSED as the owner.  */
blast_t *blast_create (float x, float y, float radius, int damage, client_id_t owner)
{
    blast_t *b = alloc (sizeof *b);

    b->x = x;
    b->y = y;
    b->max_radius = radius;
    b->max_damage = damage;
    b->owner = owner;
    if (b->owner != OBJID_CLIENT_PROCESSED)
	b->already_hit = create_vector (10);

    return b;
}


void blast_destroy (blast_t *blast)
{
    if (blast->already_hit)
	free_vector (blast->already_hit);
    free (blast);
}


static inline void do_blast_check (blast_t *blast, list_head_t *object_list)
{
    object_t *obj;
    float dx, dy;
    float manhattan_dist;
    int dmg;

    list_for_each (obj, object_list) {
	if (object_stale (obj) || object_hidden (obj))
	    continue;

	dx = object_x (obj) - blast->x;
	dy = object_y (obj) - blast->y;
	manhattan_dist = (dx * dx) + (dy * dy);
	
	if ((manhattan_dist < blast->r * blast->r) &&
	    (!in_vector_p (blast->already_hit, obj))) {

	    dmg = (blast->max_damage *
		   (1.5 - (fast_fsqrt (manhattan_dist) / blast->max_radius)));
	    dmg = MIN (dmg, blast->max_damage);
	    if (dmg > 0) {
		lua_pushnumber (server_lua_namespace, dmg);
		lua_pushnumber (server_lua_namespace, blast->owner);
		object_call (server_lua_namespace, obj, "receive_damage", 2);

		if (object_is_client (obj)) {
		    double angle = atan2 (dy, dx);
		    /* XXX: need different bounce factors for different damage */
		    object_add_extrinsic_xaya (obj, 2.3 * cos (angle), 18 * sin (angle));
		}
	    }

	    add_to_vector (blast->already_hit, obj);
	}
    }
}


/* This is for servers.  Keep in sync with below.  */
int blast_update_with_collisions (blast_t *blast, list_head_t *object_list)
{
    blast->r += SPREAD_SPEED;
    do_blast_check (blast, object_list);
    return (blast->r >= blast->max_radius) ? -1 : 0;
}


/* This is for clients.  Keep in sync with above.  */
int blast_update_visually_only (blast_t *blast)
{
    blast->r += SPREAD_SPEED;
    return (blast->r >= blast->max_radius) ? -1 : 0;
}


void blast_draw (BITMAP *dest, blast_t *blast, int offset_x, int offset_y)
{
    /* XXX this whole function is trash */

    set_add_blender (0, 0, 0, 5);
    drawing_mode (DRAW_MODE_TRANS,0,0,0);

    {
	int br = (int) (7 + 8 * (1. - blast->r/blast->max_radius)) << 4;
	br |= 0xf;

	dest->cl /= 3;
	dest->cr /= 3;
    
	circlefill (dest, blast->x - offset_x, blast->y - offset_y, blast->r,
		   makecol24 (br, br, br));

	circle (dest, blast->x - offset_x, blast->y - offset_y, blast->r,
	       makecol24 (br, br/2, 0));

	dest->cl *= 3;
	dest->cr *= 3;
    }
    
    drawing_mode (DRAW_MODE_SOLID,0,0,0);
}



/*
 *----------------------------------------------------------------------
 *	Helpers
 *----------------------------------------------------------------------
 */


struct vector {
    int num_slots;
    int used_slots;
    void **slots;
};


static vector_t *create_vector (int nslots)
{
    vector_t *vec = alloc (sizeof *vec);
    vec->slots = alloc (nslots * sizeof (void *));
    vec->num_slots = nslots;
    vec->used_slots = 0;
    return vec;
}


static void free_vector (vector_t *vec)
{
    free (vec->slots);
    free (vec);
}


static void add_to_vector (vector_t *vec, void *p)
{
    if (vec->used_slots == vec->num_slots) {
	void *q = realloc (vec->slots, sizeof (void *) * (vec->num_slots + 5));
	if (!q)
	    error ("Out of memory in blast.c\n");
	vec->slots = q;
	vec->num_slots += 5;
    }

    vec->slots[vec->used_slots] = p;
    vec->used_slots++;
}


static int in_vector_p (vector_t *vec, void *p)
{
    int i;
    
    for (i = 0; i < vec->used_slots; i++)
	if (vec->slots[i] == p)
	    return 1;

    return 0;
}
