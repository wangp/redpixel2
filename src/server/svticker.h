#ifndef __included_svticker_h
#define __included_svticker_h


typedef struct svticker svticker_t;


svticker_t *svticker_create (unsigned int ticks_per_sec);
int svticker_poll (svticker_t *t);
void svticker_destroy (svticker_t *t);
uint32_t svticker_ticks (svticker_t *t);


#endif
