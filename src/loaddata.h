#ifndef __included_loaddata_h
#define __included_loaddata_h


void tiles_load ();
void tiles_unload ();
void tiles_enumerate (void (*proc) (const char *filename, int id));

void lights_load ();
void lights_unload ();
void lights_enumerate (void (*proc) (const char *filename, int id));


#endif
