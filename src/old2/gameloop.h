#ifndef __included_gameloop_h
#define __included_gameloop_h


#include "object.h"


extern object_t *local_player;

extern int game_quit;


int game_loop_init ();
void game_loop_shutdown ();
int game_loop_update ();


#endif
