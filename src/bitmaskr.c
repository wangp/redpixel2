/* bitmaskr.c - bitmask references
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "alloc.h"
#include "bitmask.h"
#include "bitmaskr.h"
#include "list.h"


/*
 * Bitmasks come from different sources: datafiles and automatically
 * generated.  Automatically generated bitmasks need to be freed,
 * datafile bitmasks must not be.  So we add a layer above bitmask_t.
 * We also need a reference count because bitmasks are referred to by
 * many objects at once, or by the same object many times.
 */

struct bitmask_ref {
    struct bitmask_ref *next;
    struct bitmask_ref *prev;
    bitmask_t *mask;
    int ref_count;
    int free;
};


static struct list_head ref_list;


static bitmask_ref_t *create (bitmask_t *mask, int free)
{
    bitmask_ref_t *ref;
    
    ref = alloc (sizeof *ref);
    ref->mask = mask;
    ref->ref_count = 1;
    ref->free = free;
    add_to_list (ref_list, ref);
    return ref;
}


static void destroy (bitmask_ref_t *ref)
{
    if (!ref)
	return;

    del_from_list (ref);
    if (ref->free)
	bitmask_destroy (ref->mask);
    free (ref);
}


int bitmask_ref_init ()
{
    init_list (ref_list);
    return 0;
}


void bitmask_ref_shutdown ()
{
    free_list (ref_list, destroy);
}


static bitmask_ref_t *find_bitmask (bitmask_t *mask)
{
    bitmask_ref_t *ref;

    foreach (ref, ref_list)
	if (mask == ref->mask)
	    return ref;

    return NULL;
}   


bitmask_ref_t *bitmask_ref_create (bitmask_t *mask, int free)
{
    bitmask_ref_t *ref;

    ref = find_bitmask (mask);
    if (ref)
	ref->ref_count++;
    else
	ref = create (mask, free);
    return ref;
}


void bitmask_ref_destroy (bitmask_ref_t *ref)
{
    if (ref) {
	ref->ref_count--;
	if (ref->ref_count == 0)
	    destroy (ref);
    }
}


bitmask_t *bitmask_ref_bitmask (bitmask_ref_t *ref)
{
    return ref->mask;
}