/* loaddata.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "extdata.h"
#include "loaddata.h"
#include "path.h"
#include "store.h"
#include "vtree.h"


static void *loadhelp_load (void *filenames, const char *_vtree);
static void loadhelp_unload (void *ref);
static void loadhelp_enumerate (void *ref, void (*proc) (const char *filename, int id));


/*------------------------------------------------------------*/


static void *tiles;

void tiles_load ()
{
    tiles = loadhelp_load ("tile/*.dat", VTREE_TILES);
}

void tiles_unload ()
{
    loadhelp_unload (tiles);
}

void tiles_enumerate (void (*proc) (const char *filename, int id))
{
    loadhelp_enumerate (tiles, proc);
}


/*------------------------------------------------------------*/


static void *lights;

void lights_load ()
{
    lights = loadhelp_load ("light/*.dat", VTREE_LIGHTS);
}

void lights_unload ()
{
    loadhelp_unload (lights);
}

void lights_enumerate (void (*proc) (const char *filename, int id))
{
    loadhelp_enumerate (lights, proc);
}


/*------------------------------------------------------------*/


/* This is the code that actually does the work.  The weird
 * interfacing is because this used to be in a separate file, and I'm
 * too lazy to change it and explain properly why.  */


struct file {
    struct file *next;
    char *filename;
    int id;
};


/* Not reentrant, as you can see.  */
static struct file *file_list;
static const char *vtree;


static void loader (const char *filename, int attrib, int param)
{
    struct file *f;
    int id;

    id = store_load_ex (filename, vtree, load_extended_datafile);
    if (id < 0) return;
    
    f = alloc (sizeof *f);
    if (!f) {
	store_unload (id);
	return;
    }

    f->filename = ustrdup (filename);
    f->id = id;
    
    f->next = file_list->next;
    file_list->next = f;
}
   

static void *loadhelp_load (void *filenames, const char *_vtree)
{
    char **p, tmp[1024];

    file_list = alloc (sizeof *file_list);
    if (!file_list) return 0;
    
    vtree = _vtree;
    
    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filenames, sizeof tmp);
	for_each_file (tmp, FA_RDONLY | FA_ARCH, loader, 0);
    }

    return file_list;
}


static void loadhelp_unload (void *ref)
{
    struct file *file_list = ref;
    struct file *f, *next;

    for (f = file_list->next; f; f = next) {
	next = f->next;
	free (f->filename);
	store_unload (f->id);
	free (f);
    }

    free (file_list);
}


static void loadhelp_enumerate (void *ref, void (*proc) (const char *filename, int id))
{
    struct file *list = ref, *f;

    for (f = list->next; f; f = f->next)
	proc (f->filename, f->id);
}
