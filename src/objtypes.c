/* objtypes.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <lua.h>
#include "alloc.h"
#include "objtypes.h"


typedef object_type_t type_t;


static int num;
static type_t **types;


static type_t *create (const char *name, int table, const char *icon)
{
    type_t *p, **tmp;

    p = alloc (sizeof *p);
    if (!p) return 0;

    tmp = realloc (types, sizeof (type_t *) * (num + 1));
    if (!tmp) {
	free (p);
	return 0;
    }
    
    p->name  = ustrdup (name);
    p->table = table;
    p->icon  = ustrdup (icon);
    
    types = tmp;
    types[num++] = p;
    
    return p;
}


static void destroy_all ()
{
    int i;

    for (i = 0; i < num; i++) {
	free (types[i]->name);
	lua_unref (types[i]->table);
	free (types[i]->icon);
	free (types[i]);
    }

    free (types);
}


void object_types_register (const char *name, lua_Object table,
			    const char *type, const char *icon)
{
    int ref;

    lua_pushobject (table);
    ref = lua_ref (1);

    create (name, ref, icon);
}


void object_types_init ()
{
    types = 0;
    num = 0;
}


void object_types_shutdown ()
{
    destroy_all ();
}


object_type_t *object_type (const char *name)
{
    int i;

    for (i = 0; i < num; i++)
	if (!ustrcmp (types[i]->name, name))
	    return types[i];

    return 0;
}


object_type_t *object_type_from_icon (const char *icon)
{
    int i;

    for (i = 0; i < num; i++)
	if (!ustrcmp (types[i]->icon, icon))
	    return types[i];

    return 0;
}
