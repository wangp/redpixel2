#ifndef __included_loaddata_h
#define __included_loaddata_h


#include "store.h"


int tiles_init (void);
int tiles_load (const char *filename, const char *prefix);
void tiles_shutdown (void);
void tiles_enumerate (void (*proc) (const char *prefix, store_file_t));

int lights_init (void);
int lights_load (const char *filename, const char *prefix);
void lights_shutdown (void);
void lights_enumerate (void (*proc) (const char *prefix, store_file_t));


#endif
