#ifndef __included_hash_h
#define __included_hash_h


struct bucket {
    char *key;
    void *data;
    struct bucket *next;
};

struct hash_table {    
    int size;
    struct bucket **table;
};


struct hash_table *hash_construct(struct hash_table *table, int size);
void *hash_insert(const char *key, void *data, struct hash_table *table);
void *hash_lookup(const char *key, struct hash_table *table);
void *hash_del(const char *key, struct hash_table *table);
void hash_enumerate(struct hash_table *table, void (*callback)(const char *, void *));
void hash_free(struct hash_table *table, void (*func)(void *));


#endif
