#ifndef __included_loaddata_h
#define __included_loaddata_h


int tiles_init ();
int tiles_load (const char *filename, const char *prefix);
void tiles_shutdown ();
void tiles_enumerate (void (*proc) (const char *prefix, int id));

int lights_init ();
int lights_load (const char *filename, const char *prefix);
void lights_shutdown ();
void lights_enumerate (void (*proc) (const char *prefix, int id));


#endif
