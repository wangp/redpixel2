#ifndef __included_store_h
#define __included_store_h

#ifdef __cplusplus
extern "C" {
#endif


#include <allegro.h>


typedef struct store_file *store_file_t;

typedef int store_index_t;
#define STORE_INDEX_NONEXISTANT (-1)

extern DATAFILE **store;

int store_init (unsigned int size);
void store_shutdown (void);

store_file_t store_load (AL_CONST char *filename, AL_CONST char *prefix);
store_file_t store_load_ex (AL_CONST char *filename, AL_CONST char *prefix,
			    DATAFILE *(*loader) (AL_CONST char *)); /* Lua binding */
void store_unload (store_file_t f);

store_index_t store_get_index (AL_CONST char *key);
AL_CONST char *store_get_key (store_index_t index);
DATAFILE *store_get_datafile (AL_CONST char *key);
void *store_get_dat (AL_CONST char *key);

DATAFILE *store_get_file (store_file_t f);

#ifdef __cplusplus
}
#endif

#endif /* __included_store_h */
