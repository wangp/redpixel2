#ifndef __included_blast_h
#define __included_blast_h


struct list_head;
struct BITMAP;


typedef struct blast blast_t;


blast_t *blast_create (float x, float y, float radius, int damage, int owner, int visual_only);
void blast_destroy (blast_t *blast);
int blast_update (blast_t *blast, struct list_head *object_list);
void blast_draw (struct BITMAP *dest, blast_t *blast, int offset_x, int offset_y);


#endif
