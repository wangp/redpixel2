#ifndef __included_svticker_h
#define __included_svticker_h


typedef struct svticker svticker_t;


svticker_t *svticker_create (int ticks_per_sec);
int svticker_poll (svticker_t *t);
void svticker_destroy (svticker_t *t);
unsigned long svticker_ticks (svticker_t *t);


#endif
