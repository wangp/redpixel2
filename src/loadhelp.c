/* loadhelp.c - worker for loaddata.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "loadhelp.h"
#include "magicld.h"
#include "path.h"
#include "store.h"
#include "vtree.h"


struct file {
    int id;
    struct file *next;
};


/* Not reentrant, as you can see.  */
static struct file *file_list;
static const char *vtree;
static void (*loader_hook) (const char *filename, int id);


static void loader (const char *filename, int attrib, int param)
{
    struct file *f;
    int id;

    id = store_load_ex (filename, vtree, load_magic_datafile);
    if (id < 0) return;
    
    f = alloc (sizeof *f);
    if (!f) {
	store_unload (id);
	return;
    }

    f->id = id;
    f->next = file_list->next;
    file_list->next = f;

    if (loader_hook)
	loader_hook (filename, id);
}
   

void *loadhelp_load (void *filenames, const char *_vtree,
		     void (*hook) (const char *filename, int id))
{
    char **p, tmp[1024];

    file_list = alloc (sizeof *file_list);
    if (!file_list) return 0;
    
    vtree = _vtree;
    loader_hook = hook;

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filenames, sizeof tmp);
	for_each_file (tmp, FA_RDONLY | FA_ARCH, loader, 0);
    }

    return file_list;
}


void loadhelp_unload (void *ref)
{
    struct file *file_list = ref;
    struct file *f, *next;

    for (f = file_list->next; f; f = next) {
	next = f->next;
	store_unload (f->id);
	free (f);
    }

    free (file_list);
}
