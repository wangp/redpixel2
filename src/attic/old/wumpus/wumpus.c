/*  Wumpus, by Peter Wang <tjaden@users.sourceforge.net>
 *  See README for information.
 */


#include <string.h>
#include <allegro.h>
#include "wumpus.h"


static int count_datafile(DATAFILE *d)
{
    int i, c;
    
    for (i = c = 0; d[i].type != DAT_END; i++, c++)
	if (d[i].type == DAT_FILE)
	    c += count_datafile(d[i].dat);
    return c;
}


/* XXX: get rid of this path thing */
/* XXX: also, unicode-awarify */
static char path[1024];

static void build(DATAFILE *d, HASH_TABLE *hash, char *basepath)
{
    char *name;
    int i;
    
    for (i = 0; d[i].type != DAT_END; i++) {
	name = get_datafile_property(&d[i], DAT_NAME);
	if (strcmp(name, "") == 0
	    || strcmp(name, "GrabberInfo") == 0)
	    continue;
	
	if (d[i].type == DAT_FILE) {
	    strcpy(basepath, name);
	    strcat(basepath, "/");
	    build(d[i].dat, hash, basepath+strlen(name)+1);
	}
	
	strcpy(basepath, name);
	hash_insert(path, &d[i], hash);
    }
}


static void unbuild(DATAFILE *d, HASH_TABLE *hash, char *basepath)
{
    char *name;
    int i;
    
    for (i = 0; d[i].type != DAT_END; i++) {
	name = get_datafile_property(&d[i], DAT_NAME);
	if (strcmp(name, "") == 0)
	    continue;
	
	if (d[i].type == DAT_FILE) {
	    strcpy(basepath, name);
	    strcat(basepath, "/");
	    unbuild(d[i].dat, hash, basepath+strlen(name)+1);
	}
	
	strcpy(basepath, name);
	hash_del(path, hash);
    }
}


/* rebuild_wumpus_hash:
 *  Rebuild the wumpus hash table with contents of the datafile.
 */
void rebuild_wumpus_hash(WUMPUS *w)
{
    int c;
    
    if (!w->df) return;
    
    if (w->hash.size)
	hash_free(&w->hash, NULL);

    c = count_datafile(w->df);
    hash_construct(&w->hash, c);
    if (w->hash.size) {
	path[0] = '\0';
	build(w->df, &w->hash, path);
    }
}

 
/* load_wumpus:
 *  Loads a datafile, hashes it, then returns a pointer to 
 *  the WUMPUS structure, or NULL on error.
 */
WUMPUS *load_wumpus(const char *filename)
{
    DATAFILE *d;
    WUMPUS *w;
        
    d = load_datafile((char *) filename);
    if (!d)
	return NULL;
    
    w = malloc(sizeof(WUMPUS));
    if (!w) {
	unload_datafile(d);
	return NULL;
    }
    
    w->df = d;
    w->hash.size = 0;
    
    rebuild_wumpus_hash(w);
    return w;
}


/* unload_wumpus:
 *  Unloads the datafile inside a wumpus, and frees the hash table. 
 */
void unload_wumpus(WUMPUS *w)
{
    if (w) {
	if (w->df) {
	    unload_datafile(w->df);
	    w->df = NULL;
	}
	
	hash_free(&w->hash, NULL);
    }
}


/* These are used to keep track of the `global' hash table and the
 * datafiles contained within it.  */

HASH_TABLE fat_gellopy;

typedef struct NODE {
    DATAFILE *d;
    char *name;
    struct NODE *next;
} NODE;

static NODE *list;


static void insert_node(DATAFILE *d, char *name)
{
    NODE *it, *nu;
    
    nu = malloc(sizeof(NODE));
    if (!nu) return;
    
    /* We link it in at the end of the list so if there are name collisions,
     * and we need to rebuild the hash table later, files linked in later will
     * override the ones linked earlier.
     */
    
    nu->d = d;
    nu->name = strdup(name);
    nu->next = NULL;

    if (!list)
	list = nu;
    else {
	it = list;
	while (it->next)
	    it = it->next;
	it->next = nu;
    }
}


static void remove_node(DATAFILE *d)
{
    NODE *it = list, *prev = NULL;
    while (it) {
	if (it->d == d) {
	    if (prev)
		prev->next = it->next;
	    else 
		list = it->next;
	    
	    free(it->name);
	    free(it);
	    return;
	}
	
	prev = it;
	it = it->next;	
    }
}


static NODE *find_node(DATAFILE *d)
{
    NODE *it;

    for (it = list; it; it = it->next)
	if (it->d == d)
	    return it;

    return NULL;
}


/* install_gellopy:
 *  Sets the global hash table at a specified size.
 */
int install_gellopy(int size)
{
    if (!hash_construct(&fat_gellopy, size))
	return FALSE;
    atexit(remove_gellopy); 
    return TRUE;
}


/* remove_gellopy:
 *  Frees memory required by the gellopy.
 */
void remove_gellopy()
{
    if (fat_gellopy.size)
	hash_free(&fat_gellopy, NULL);
    
    while (list) 
	remove_node(list->d);
}

 
/* link_datafile:
 *  Links and hashes a datafile into internal table.
 */
void link_datafile(DATAFILE *d, const char *basename)
{
    int i;
    
    if (!d) return;
    
    i = strlen(basename);
    strcpy(path, basename);
    if (basename[i-1] != '/')
	strcat(path, "/");
    
    insert_node(d, path);
    build(d, &fat_gellopy, path + strlen(path));
}


static void rebuild_all()
{
    NODE *it = list;
    while (it) {
	strcpy(path, it->name);
	build(it->d, &fat_gellopy, path + strlen(path));
	it = it->next;
    }
}


/* unlink_datafile:
 *  Unlinks hashed a datafile from internal table.
 */
void unlink_datafile(DATAFILE *d)
{
    NODE *it = find_node(d);
    
    if (it) {
	strcpy(path, it->name);
	unbuild(d, &fat_gellopy, path + strlen(path));
	rebuild_all();
	remove_node(d);
    }
}


/* link_wumpus:
 *  Convenience function.
 */
void link_wumpus(WUMPUS *w, const char *basename)
{
    if (w)
	link_datafile(w->df, basename);
}


/* unlink_wumpus:
 *  Convenience function.
 */
void unlink_wumpus(WUMPUS *w)
{
    if (w)
	unlink_datafile(w->df);
}
