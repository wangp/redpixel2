#ifndef __included_loaddata_h
#define __included_loaddata_h


void tiles_load (void (*hook) (const char *filename, int id));
void tiles_unload ();

void lights_load (void (*hook) (const char *filename, int id));
void lights_unload ();


#endif
