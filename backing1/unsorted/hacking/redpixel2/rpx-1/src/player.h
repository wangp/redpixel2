#ifndef _included_player_h_
#define _included_player_h_

#include <allegro.h>

#define MAX_PLAYERS	32	       /* ha! I'm dreaming... */

typedef struct player
{
    /* $-start-struct player_t */
    
    int x, y;
    int vx, vy;			       /* velocity */
    
    int legframe, legtics;
    
    BITMAP *bmp;
    
    /* $-end-struct */
} player_t;

extern player_t player[];

#endif
