#ifndef __included_blast_h
#define __included_blast_h


#include "id.h"

struct list_head;
struct BITMAP;
struct lua_State;


typedef struct blast blast_t;


blast_t *blast_create (float x, float y, float radius, int damage, client_id_t owner);
void blast_destroy (blast_t *blast);
int blast_update_with_collisions (blast_t *blast, struct list_head *object_list);
int blast_update_visually_only (blast_t *blast);
void blast_draw (struct BITMAP *dest, blast_t *blast, int offset_x, int offset_y);


#endif
