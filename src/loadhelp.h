#ifndef __included_loadhelp_h
#define __included_loadhelp_h


void *loadhelp_load (void *filenames, const char *_vtree,
		     void (*hook) (const char *filename, int id));
void loadhelp_unload (void *ref);



#endif
