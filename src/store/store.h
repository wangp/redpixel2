#ifndef __included_store_h
#define __included_store_h

#ifdef __cplusplus
extern "C" {
#endif


extern DATAFILE **store;


int store_init (int size);
void store_shutdown (void);

int store_load (const char *filename, const char *prefix);
int store_load_ex (const char *filename, const char *prefix, DATAFILE *(*loader) (const char *));
void store_unload (int id);

int store_index (const char *key);
char *store_key (int index);
DATAFILE *store_datafile (const char *key);
void *store_dat (const char *key);

DATAFILE *store_file (int id);
    

#ifdef __cplusplus
}
#endif

#endif /* __included_store_h */
