/* loaddata.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "extdata.h"
#include "list.h"
#include "loaddata.h"
#include "path.h"
#include "store.h"


typedef struct file_list file_list_t;

struct file_list {
    file_list_t *next;
    file_list_t *prev;
    char *prefix;
    int id;
};


/*------------------------------------------------------------*/


static list_head_t tiles_list;

int tiles_init (void)
{
    list_init (tiles_list);
    return 0;
}

int tiles_load (const char *filename, const char *prefix)
{
    file_list_t *f;
    int id;
    
    id = store_load_ex (filename, prefix, load_extended_datafile);
    if (id < 0)
	return -1;

    f = alloc (sizeof *f);
    f->prefix = ustrdup (prefix);
    f->id = id;
    list_add (tiles_list, f);
    return 0;
}

void tiles_shutdown (void)
{
    file_list_t *f;

    list_for_each (f, &tiles_list) {
	free (f->prefix);
	store_unload (f->id);
    }

    list_free (tiles_list, free);
}

void tiles_enumerate (void (*proc) (const char *prefix, int id))
{
    file_list_t *f;

    list_for_each (f, &tiles_list)
	proc (f->prefix, f->id);
}


/*------------------------------------------------------------*/


/* s/tiles/lights/g */

static list_head_t lights_list;

int lights_init (void)
{
    list_init (lights_list);
    return 0;
}

int lights_load (const char *filename, const char *prefix)
{
    file_list_t *f;
    int id;
    
    id = store_load_ex (filename, prefix, load_extended_datafile);
    if (id < 0)
	return -1;

    f = alloc (sizeof *f);
    f->prefix = ustrdup (prefix);
    f->id = id;
    list_add (lights_list, f);
    return 0;
}

void lights_shutdown (void)
{
    file_list_t *f;

    list_for_each (f, &lights_list) {
	free (f->prefix);
	store_unload (f->id);
    }

    list_free (lights_list, free);
}

void lights_enumerate (void (*proc) (const char *prefix, int id))
{
    file_list_t *f;

    list_for_each (f, &lights_list)
	proc (f->prefix, f->id);
}
