/* store.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "store.h"


/*----------------------------------------------------------------------*/

/*
 * This tracks the list of files that are currently loaded.
 */

struct store_file {
    store_file_t next;
    char *prefix;
    DATAFILE *dat;
};

static struct store_file file_list_head;

static void
init_file_list (void)
{
    file_list_head.next = NULL;
}

static store_file_t
add_to_file_list (DATAFILE *dat, AL_CONST char *prefix)
{
    store_file_t f = malloc (sizeof *f);

    if (f) {
	f->prefix = strdup (prefix);
	f->dat = dat;
	f->next = file_list_head.next;
	file_list_head.next = f;
	return f;
    }
    else
	return NULL;
}

static void
remove_from_file_list (store_file_t f)
{
    store_file_t prev;

    for (prev = &file_list_head; prev != NULL; prev = prev->next) {
	if (prev->next == f) {
	    prev->next = f->next;
	    free (f->prefix);
	    free (f);
	    return;
	}
    }
}

static void
free_file_list (void)
{
    while (file_list_head.next)
	remove_from_file_list (file_list_head.next);
}


/*----------------------------------------------------------------------*/

/*
 * This maintains a big array (called "the store") of all the datafile
 * items that have been loaded.
 */

DATAFILE **store;		/* PUBLIC SYMBOL */

#define the_store store		/* for less confusion, internally */
static store_index_t the_store_size;

static void
init_the_store (void)
{
    the_store = NULL;
    the_store_size = 0;
}

static void
free_the_store (void)
{
    free (the_store);
}

static store_index_t
add_item_to_the_store (DATAFILE *item)
{
    store_index_t i;
    void *p;

    /* If we are refreshing after a store_unload() call, item may
     * already exist.  */
    for (i = 0; i < the_store_size; i++)
	if (the_store[i] == item)
	    return i;

    /* Otherwise it's a new item.  */
    p = realloc (the_store, (the_store_size + 1) * sizeof (DATAFILE *));
    if (p) {
	store_index_t n = the_store_size;
	the_store_size++;
	the_store = p;
	the_store[n] = item;
	return n;
    }

    /* Out of memory? */
    return STORE_INDEX_NONEXISTANT;
}

/* Clear any addresses in `store' which are in `d', and its children
 * if `d' is a sub-datafile. */
static void
clear_from_the_store (DATAFILE *d)
{
    int i;
    store_index_t j;

    for (i = 0; d[i].type != DAT_END; i++) {
	for (j = 0; j < the_store_size; j++) {
	    if (the_store[j] == &d[i]) {
		the_store[j] = NULL;
		break;
	    }
	}

	if (d[i].type == DAT_FILE)
	    clear_from_the_store (d[i].dat);
    }
}


/*----------------------------------------------------------------------*/

/*
 * This maintains a hash table for fast lookup of datafile items in
 * the store.
 */

#include "bjhash.inc"

typedef struct bucket {
    struct bucket *next;
    char *key;
    store_index_t value;
}  bucket_t;

static struct {
    unsigned int size;
    unsigned int hash_mask;
    bucket_t **buckets;
} lookup_table;

static unsigned int
good_hash_size (unsigned int k, unsigned int *maskret)
{
    unsigned int n = 1;
    while (k > hashsize (n)) n++;
    *maskret = hashmask (n);
    return hashsize (n);
}

static int
init_lookup_table (unsigned int size)
{
    unsigned int hash_mask;
    size = good_hash_size (size, &hash_mask);

    lookup_table.buckets = calloc (size, sizeof (bucket_t *));
    if (lookup_table.buckets) {
	lookup_table.size = size;
	lookup_table.hash_mask = hash_mask;
	return 0;
    }
    else
	return -1;
}

static void
clear_lookup_table (void)
{
    unsigned int i;
    bucket_t *bkt;
    bucket_t *next;

    for (i = 0; i < lookup_table.size; i++) {
	for (bkt = lookup_table.buckets[i]; bkt != NULL; bkt = next) {
	    next = bkt->next;
	    free (bkt->key);
	    free (bkt);
	}
	lookup_table.buckets[i] = NULL;
    }
}

static void
free_lookup_table (void)
{
    if (lookup_table.size) {
	clear_lookup_table ();
	free (lookup_table.buckets);
	lookup_table.size = 0;
    }
}

#define hash_string(s)	(hash(s, strlen(s), 1))

static void
really_add_to_lookup_table (AL_CONST char *key, store_index_t value)
{
    unsigned int i = hash_string(key) & lookup_table.hash_mask;
    bucket_t *bkt;

    /* Check if key already exists.  */
    for (bkt = lookup_table.buckets[i]; bkt != NULL; bkt = bkt->next)
	if (strcmp(key, bkt->key) == 0)
	    return;

    /* Add new bucket.  */
    bkt = malloc(sizeof *bkt);
    bkt->key = strdup(key);
    bkt->value = value;
    bkt->next = lookup_table.buckets[i];
    lookup_table.buckets[i] = bkt;
}

#define DAT_INFO  DAT_ID('i','n','f','o')

static void
add_to_lookup_table (DATAFILE *d, AL_CONST char *prefix)
{
    AL_CONST char *name;
    char path[1024];
    unsigned int i;
    store_index_t k;

    for (i = 0; d[i].type != DAT_END; i++) {
	if (d[i].type == DAT_INFO)
	    continue;

	name = get_datafile_property (&d[i], DAT_NAME);
	if (!name[0])
	    continue;

	strncpy (path, prefix, sizeof path); path[(sizeof path) - 1] = '\0';
	strncat (path, name, sizeof path - strlen (path) - 1);

	k = add_item_to_the_store (&d[i]);
	if (k != STORE_INDEX_NONEXISTANT) {
	    really_add_to_lookup_table (path, k);

	    if (d[i].type == DAT_FILE) {
		strncat (path, "/", sizeof path - strlen (path) - 1);
		add_to_lookup_table (d[i].dat, path);
	    }
	}
    }
}

static store_index_t
find_in_lookup_table (const char *key)
{
    unsigned int i = hash_string(key) & lookup_table.hash_mask;
    bucket_t *bkt;

    for (bkt = lookup_table.buckets[i]; bkt != NULL; bkt = bkt->next)
	if (strcmp(key, bkt->key) == 0)
	    return bkt->value;

    return STORE_INDEX_NONEXISTANT;
}


/*----------------------------------------------------------------------*/

/*
 * This is the public interface.
 */

int
store_init (unsigned int size)
{
    init_file_list ();
    init_the_store ();
    if (init_lookup_table (size) == 0)
	return 0;
    else
	return -1;
}

void
store_shutdown (void)
{
    store_file_t p;

    free_lookup_table ();
    free_the_store ();
    for (p = file_list_head.next; p; p = p->next)
	unload_datafile (p->dat);
    free_file_list ();
}

store_file_t
store_load_ex (AL_CONST char *filename, AL_CONST char *prefix,
	       DATAFILE *(*loader) (AL_CONST char *))
{
    DATAFILE *d = loader (filename);

    if (d) {
	store_file_t f = add_to_file_list (d, prefix);
	if (f)
	    add_to_lookup_table (d, prefix);
	else
	    unload_datafile (d);
	return f;
    }
    else
	return NULL;
}

store_file_t
store_load (AL_CONST char *filename, AL_CONST char *prefix)
{
    return store_load_ex (filename, prefix, load_datafile);
}

void
store_unload (store_file_t f)
{
    store_file_t p;

    clear_from_the_store (f->dat);
    unload_datafile (f->dat);
    remove_from_file_list (f);

    clear_lookup_table ();
    for (p = file_list_head.next; p != NULL; p = p->next)
	add_to_lookup_table (p->dat, p->prefix);
}

inline store_index_t
store_get_index (AL_CONST char *key)
{
    return find_in_lookup_table (key);
}

AL_CONST char *
store_get_key (store_index_t index)
{
    unsigned int i;
    bucket_t *bkt;

    for (i = 0; i < lookup_table.size; i++)
	for (bkt = lookup_table.buckets[i]; bkt != NULL; bkt = bkt->next)
	    if (bkt->value == index)
		return bkt->key;

    return NULL;
}

inline DATAFILE *
store_get_datafile (AL_CONST char *key)
{
    store_index_t n = store_get_index (key);
    return (n != STORE_INDEX_NONEXISTANT) ? store[n] : NULL;
}

void *
store_get_dat (AL_CONST char *key)
{
    DATAFILE *d = store_get_datafile (key);
    return (d) ? d->dat : NULL;
}

DATAFILE *
store_get_file (store_file_t f)
{
    return f->dat;
}
