/* objtypes.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "mylua.h"
#include "objtypes.h"


static int num;
static objtype_t **types;


static objtype_t *create (const char *type, const char *name,
			  const char *icon, lref_t init_func)
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

    types = tmp;
    types[num++] = p;

    return p;
}


static void destroy_all ()
{
    int i;

    for (i = 0; i < num; i++) {
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
    create (type, name, icon, init_func);
}


objtype_t *objtypes_lookup (const char *name)
{
    int i;

    for (i = 0; i < num; i++)
	if (!ustrcmp (types[i]->name, name))
	    return types[i];

    return 0;
}


void objtypes_enumerate (void (*proc) (objtype_t *type))
{
    int i;

    for (i = 0; i < num; i++)
	proc (types[i]);
}
