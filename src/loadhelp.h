#ifndef __included_loadhelp_h
#define __included_loadhelp_h


void *loadhelp_load (void *filenames, const char *_vtree);
void loadhelp_unload (void *ref);
void loadhelp_enumerate (void *ref, void (*proc) (const char *filename, int id));


#endif
