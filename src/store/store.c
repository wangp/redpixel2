/* store.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "hash.h"
#include "store.h"


#define DAT_INFO  DAT_ID('i','n','f','o')


struct file {
    int id;
    char *prefix;
    DATAFILE *dat;
    struct file *next;
};

static int file_id;
static struct file file_list;

static void list_init (void)
{
    file_list.next = 0;
    file_id = 0;
}

static int list_add (DATAFILE *dat, AL_CONST char *prefix)
{
    struct file *p, *f = malloc (sizeof *f);

    if (f) {
	f->id = file_id++;
	f->prefix = strdup (prefix);
	f->dat = dat;
	f->next = 0;

	for (p = &file_list; p->next; p = p->next)
	    ;
	p->next = f;
    }

    return (f) ? (f->id) : -1;
}

static struct file *list_find (int id, struct file **prevret)
{
    struct file *p;

    for (p = &file_list; p->next; p = p->next)
	if (p->next->id == id) {
	    if (prevret)
		*prevret = p;
	    return p->next;
	}

    return 0;
}

static void list_remove (int id)
{
    struct file *p, *prev;

    p = list_find (id, &prev);

    if (p) {
	prev->next = p->next;
	free (p->prefix);
	free (p);
    }
}

static void list_shutdown (void)
{
    while (file_list.next)
	list_remove (file_list.next->id);
}

/*----------------------------------------------------------------------*/

static int idx;
DATAFILE **store;

static void data_init (void)
{
    store = 0;
    idx = 1;
}

static void data_shutdown (void)
{
    free (store);
}

static int data_append (DATAFILE *item)
{
    int i;
    void *p;
    
    /* if we are refreshing after a store_unload() call, item may
     * already exist */
    for (i = 1; i < idx; i++)
	if (store[i] == item)
	    return i;

    /* otherwise it's a new item */
    p = realloc (store, (idx + 1) * sizeof (DATAFILE *));

    if (p) {
	int n = idx++;
	store = p;
	store[n] = item;
	return n;
    }
    
    return 0;
}

/* clear any addresses in `store' which are in `d' and its children */
static void data_clear (DATAFILE *d)
{
    int i, j;

    for (i = 0; d[i].type != DAT_END; i++) {
	for (j = 1; j < idx; j++) {
	    if (store[j] == &d[i]) {
		store[j] = NULL;
		break;
	    }
	}

	if (d[i].type == DAT_FILE)
	    data_clear (d[i].dat);
    }
}

/*----------------------------------------------------------------------*/

static struct hash_table table;

static int table_init (int size)
{
    return !hash_construct (&table, size) ? (-1) : 0;
}

static void table_shutdown (void)
{
    if (table.size)
	hash_free (&table, 0);
    table.size = 0;
}

static void table_add (DATAFILE *d, AL_CONST char *prefix)
{
    AL_CONST char *name;
    char path[1024];
    int i, n;

    for (i = 0; d[i].type != DAT_END; i++) {
	if (d[i].type == DAT_INFO)
	    continue;

	name = get_datafile_property (&d[i], DAT_NAME);
	if (!name[0])
	    continue;

	strncpy (path, prefix, sizeof path); path[(sizeof path) - 1] = '\0';
	strncat (path, name, sizeof path - strlen (path) - 1);

	n = data_append (&d[i]);
	if (n)
	    hash_insert (path, (void *) n, &table);
    	
	if (d[i].type == DAT_FILE) {
	    strncat (path, "/", sizeof path - strlen (path) - 1);
	    table_add (d[i].dat, path);
	}
    }
}

/*----------------------------------------------------------------------*/

int store_init (int size)
{
    list_init ();
    data_init ();
    if (table_init (size) < 0)
	return -1;
    return 0;
}

void store_shutdown (void)
{
    struct file *p;
    
    table_shutdown ();
    data_shutdown ();
    for (p = file_list.next; p; p = p->next)
    	unload_datafile (p->dat);
    list_shutdown ();
}

int store_load_ex (AL_CONST char *filename, AL_CONST char *prefix, DATAFILE *(*loader) (AL_CONST char *))
{
    DATAFILE *d;
    int id;

    d = loader (filename);
	
    if (d) {	
	id = list_add (d, prefix);
	
	if (id < 0)
	    unload_datafile (d);
	else
	    table_add (d, prefix);
	
	return id;
    }
   
    return -1;
}

int store_load (AL_CONST char *filename, AL_CONST char *prefix)
{
    return store_load_ex (filename, prefix, load_datafile);
}

static void refresh (void)
{
    int size;
    struct file *p;

    size = table.size;
    table_shutdown ();
    table_init (size);

    for (p = file_list.next; p; p = p->next)
	table_add (p->dat, p->prefix);
}

void store_unload (int id)
{
    struct file *p = list_find (id, 0);
    
    if (p) {
    	data_clear (p->dat);
	unload_datafile (p->dat);
	list_remove (id);
	refresh ();
    }
}

inline int store_index (AL_CONST char *key)
{
    int n = (int) hash_lookup (key, &table);
    return n;
}

char *store_key (int index)
{
    int i;
    struct bucket *p;

    /* easier to walk it ourselves than to use `hash_enumerate' */
    for (i = 0; i < table.size; i++) 
	for (p = table.table[i]; p; p = p->next) 
	    if ((int) p->data == index)
		return p->key;

    return 0;
}
    
inline DATAFILE *store_datafile (AL_CONST char *key)
{
    int n = store_index (key);
    return (n) ? store[n] : 0;
}
 
void *store_dat (AL_CONST char *key)
{
    DATAFILE *d = store_datafile (key);
    return (d) ? d->dat : 0;
}

DATAFILE *store_file (int id)
{
    struct file *f = list_find (id, 0);
    return (f) ? (f->dat) : 0;
}
