#ifndef __included_wumpus_h
#define __included_wumpus_h

#include <allegro.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct BUCKET {
    char *key;
    void *data;
    struct BUCKET *next;
} BUCKET;

typedef struct HASH_TABLE {    
    int size;    
    BUCKET **table;    
} HASH_TABLE;

HASH_TABLE *hash_construct(HASH_TABLE *table, int size);
void *hash_insert(const char *key, void *data, struct HASH_TABLE *table);
void *hash_lookup(const char *key, struct HASH_TABLE *table);
void *hash_del(const char *key, struct HASH_TABLE *table);
void hash_enumerate(struct HASH_TABLE *table, void (*callback)(const char *, void *));
void hash_free(HASH_TABLE *table, void (*callback)(void *));


typedef struct WUMPUS {
    DATAFILE *df;
    HASH_TABLE hash;
} WUMPUS;
    
extern HASH_TABLE fat_gellopy;

WUMPUS *load_wumpus(const char *filename);
void unload_wumpus(WUMPUS *w);
void rebuild_wumpus_hash(WUMPUS *w);

int install_gellopy(int size);
void remove_gellopy(void);

void link_datafile(DATAFILE *d, const char *basename);
void unlink_datafile(DATAFILE *d);
void link_wumpus(WUMPUS *w, const char *basename);
void unlink_wumpus(WUMPUS *w);

extern inline
DATAFILE *wumpus_datafile(WUMPUS *w, const char *key)
{
    return hash_lookup(key, &w->hash);
}

extern inline
void *wumpus_dat(WUMPUS *w, const char *key)
{
    DATAFILE *d = hash_lookup(key, &w->hash);
    if (d)
      return d->dat;
    else
      return NULL;
}

extern inline
const char *get_wumpus_property(WUMPUS *w, const char *key, int type)
{
    return get_datafile_property(wumpus_datafile(w, key), type);
}

extern inline
const char *get_wumpus_tag(WUMPUS *w, const char *key, const char *type)
{
    int id = DAT_ID(type[0], type[1], type[2], type[3]);
    return get_datafile_property(wumpus_datafile(w, key), id);
}

extern inline
DATAFILE *gellopy_datafile(const char *key)
{
    return hash_lookup(key, &fat_gellopy);
}

extern inline
void *gellopy_dat(const char *key)
{
    DATAFILE *d = hash_lookup(key, &fat_gellopy);
    if (d)
	return d->dat;
    else
	return NULL;
}

extern inline
const char *get_gellopy_property(const char *key, int type)
{
    return get_datafile_property(gellopy_datafile(key), type);
}

extern inline
const char *get_gellopy_tag(const char *key, char *type)
{
    int id = DAT_ID(type[0], type[1], type[2], type[3]);
    return get_datafile_property(gellopy_datafile(key), id);
}


#define HASHED_DATAFILE			WUMPUS
#define load_hashed_datafile		load_wumpus
#define unload_hashed_datafile		unload_wumpus
#define rebuild_hash_table		rebuild_wumpus_hash
#define install_hashed_datafile		install_gellopy
#define remove_hashed_datafile		remove_gellopy
#define link_hashed_datafile		link_wumpus
#define unlink_hashed_datafile		unlink_wumpus


#ifdef __cplusplus
}
#endif

#endif
