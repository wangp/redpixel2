#ifndef _included_player_h_
#define _included_player_h_

#define MAX_PLAYERS	32	       /* ha! I'm dreaming... */

typedef struct player
{
    /* $-start-struct player_t */
    
    unsigned int x, y;
    
    /* $-end-struct */
} player_t;

extern player_t player[];

#endif
