#ifndef __included_store_h
#define __included_store_h

#ifdef __cplusplus
extern "C" {
#endif


#include <allegro.h>


extern DATAFILE **store;


int store_init (int size);
void store_shutdown (void);

int store_load (AL_CONST char *filename, AL_CONST char *prefix);
int store_load_ex (AL_CONST char *filename, AL_CONST char *prefix,
		   DATAFILE *(*loader) (AL_CONST char *));
void store_unload (int id);

int store_index (AL_CONST char *key);
char *store_key (int index);
DATAFILE *store_datafile (AL_CONST char *key);
void *store_dat (AL_CONST char *key);

DATAFILE *store_file (int id);
    

#ifdef __cplusplus
}
#endif

#endif /* __included_store_h */
