#ifndef __included_bitmaskr_h
#define __included_bitmaskr_h


#include "bitmask.h"


typedef struct bitmask_ref bitmask_ref_t;


int bitmask_ref_init ();
void bitmask_ref_shutdown ();
bitmask_ref_t *bitmask_ref_create (bitmask_t *mask, int free);
void bitmask_ref_destroy (bitmask_ref_t *ref);
bitmask_t *bitmask_ref_bitmask (bitmask_ref_t *ref);


#endif
