/* objtypes.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "bitmaskg.h"
#include "mylua.h"
#include "objtypes.h"
#include "store.h"


static int num;
static objtype_t **types;


static objtype_t *create (const char *type, const char *name,
			  const char *icon, lref_t init_func,
			  bitmask_t *icon_mask)
{
    objtype_t *p, **tmp;

    p = alloc (sizeof *p);

    tmp = realloc (types, sizeof (objtype_t *) * (num + 1));
    if (!tmp) {
	free (p);
	return 0;
    }

    p->type = ustrdup (type);
    p->name = ustrdup (name);
    p->icon = ustrdup (icon);
    p->init_func = init_func;
    p->icon_mask = icon_mask;

    types = tmp;
    types[num++] = p;

    return p;
}


static void destroy_all ()
{
    int i;

    for (i = 0; i < num; i++) {
	bitmask_destroy (types[i]->icon_mask);
	unref (lua_state, types[i]->init_func);
	free (types[i]->icon);
	free (types[i]->name);
	free (types[i]->type);
	free (types[i]);
    }

    free (types);
}


void objtypes_init ()
{
    types = 0;
    num = 0;
}


void objtypes_shutdown ()
{
    destroy_all ();
}


void objtypes_register (const char *type, const char *name,
			const char *icon, lref_t init_func)
{
    create (type, name, icon, init_func,
	    bitmask_create_from_magic_bitmap (store_dat (icon)));
}


objtype_t *objtypes_lookup (const char *name)
{
    int i;

    for (i = 0; i < num; i++)
	if (!ustrcmp (types[i]->name, name))
	    return types[i];

    return NULL;
}


void objtypes_enumerate (void (*proc) (objtype_t *type))
{
    int i;

    for (i = 0; i < num; i++)
	proc (types[i]);
}
