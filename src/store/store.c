/* store.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "hash.h"
#include "store.h"


struct file {
    int id;
    char *prefix;
    DATAFILE *dat;
    struct file *next;
};

static int file_id;
static struct file file_list;

static void list_init ()
{
    file_list.next = 0;
    file_id = 0;
}

static int list_add (DATAFILE *dat, const char *prefix)
{
    struct file *p, *f = malloc (sizeof *f);

    if (f) {
	f->id = file_id++;
	f->prefix = ustrdup (prefix);
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

static void list_shutdown ()
{
    while (file_list.next)
	list_remove (file_list.next->id);
}

/*----------------------------------------------------------------------*/

static int index;
DATAFILE **store;

static void data_init ()
{
    store = 0;
    index = 1;
}

static void data_shutdown ()
{
    free (store);
}

static int data_append (DATAFILE *item)
{
    void *p = realloc (store, (index + 1) * sizeof (DATAFILE *));

    if (p) {
	int n = index++;
	store = p;
	store[n] = item;
	return n;
    }
    
    return 0;
}

/*----------------------------------------------------------------------*/

static struct hash_table table;

static int table_init (int size)
{
    return !hash_construct (&table, size) ? (-1) : 0;
}

static void table_shutdown ()
{
    if (table.size)
	hash_free (&table, 0);
    table.size = 0;
}

static void table_add (DATAFILE *d, const char *prefix)
{
    const char *name;
    char path[1024];
    int i, n;

    for (i = 0; d[i].type != DAT_END; i++) {
	name = get_datafile_property (&d[i], DAT_NAME);
	if (!ustrcmp (name, empty_string)
	    || !ustrcmp (name, "GrabberInfo"))
	    continue;

	ustrncpy (path, prefix, sizeof path);
	ustrncat (path, name, sizeof path);

	n = data_append (&d[i]);
	if (n)
	    hash_insert (path, (void *) n, &table);
    	
	if (d[i].type == DAT_FILE) {
	    ustrncat (path, "/", sizeof path);
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
    table_shutdown ();
    data_shutdown ();
    list_shutdown ();
}

int store_load_ex (const char *filename, const char *prefix, DATAFILE *(*loader) (const char *))
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

int store_load (const char *filename, const char *prefix)
{
    return store_load_ex (filename, prefix, load_datafile);
}

static void refresh ()
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
	unload_datafile (p->dat);
	list_remove (id);
	refresh ();
    }
}

inline int store_index (const char *key)
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
    
inline DATAFILE *store_datafile (const char *key)
{
    int n = store_index (key);
    return (n) ? store[n] : 0;
}
 
void *store_dat (const char *key)
{
    DATAFILE *d = store_datafile (key);
    return (d) ? d->dat : 0;
}

DATAFILE *store_file (int id)
{
    struct file *f = list_find (id, 0);
    return (f) ? (f->dat) : 0;
}
