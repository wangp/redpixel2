#ifndef __included_blod_h
#define __included_blod_h


struct map;


void blod_spawn (struct map *, float x, float y, long nparticles);
void blod_init ();
void blod_shutdown ();


#endif
