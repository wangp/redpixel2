/* objtypes.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <string.h>
#include <allegro.h>
#include "alloc.h"
#include "bitmask.h"
#include "bitmaskg.h"
#include "error.h"
#include "mylua.h"
#include "objtypes.h"
#include "store.h"


struct objtype {
    char *type;
    char *name;
    char *icon;
    lua_ref_t init_func;
    bitmask_t *icon_mask;
};


static int num;
static objtype_t **types;


static objtype_t *create (const char *type, const char *name,
			  const char *icon, lua_ref_t init_func,
			  bitmask_t *icon_mask)
{
    objtype_t *p, **tmp;

    p = alloc (sizeof *p);

    tmp = realloc (types, sizeof (objtype_t *) * (num + 1));
    if (!tmp) {
	free (p);
	return 0;
    }

    p->type = type ? strdup (type) : 0;
    p->name = strdup (name);
    p->icon = strdup (icon);
    p->init_func = init_func;
    p->icon_mask = icon_mask;

    types = tmp;
    types[num++] = p;

    return p;
}


static void destroy_all (void)
{
    int i;

    for (i = 0; i < num; i++) {
	bitmask_destroy (types[i]->icon_mask);
	lua_unref (the_lua_state, types[i]->init_func);
	free (types[i]->icon);
	free (types[i]->name);
	free (types[i]->type);
	free (types[i]);
    }

    free (types);
}


void objtypes_init (void)
{
    types = 0;
    num = 0;
}


void objtypes_shutdown (void)
{
    destroy_all ();
}


int objtypes_register (const char *type, const char *name,
			const char *icon, lua_ref_t init_func)
{
    BITMAP *icon_bmp;

    icon_bmp = store_get_dat (icon);
    if (!icon_bmp)
	errorv ("Bad icon %s\n", icon);

    return (create (type, name, icon, init_func,
		    bitmask_create_from_magic_bitmap (icon_bmp))
	    ? 0 : -1);
}


objtype_t *objtypes_lookup (const char *name)
{
    int i;

    for (i = 0; i < num; i++)
	if (!strcmp (types[i]->name, name))
	    return types[i];

    return NULL;
}


void objtypes_enumerate (void (*proc) (objtype_t *type))
{
    int i;

    for (i = 0; i < num; i++)
	proc (types[i]);
}


const char *objtype_type (objtype_t *type)
{
    return type->type;
}


const char *objtype_name (objtype_t *type)
{
    return type->name;
}


const char *objtype_icon (objtype_t *type)
{
    return type->icon;
}


lua_ref_t objtype_init_func (objtype_t *type)
{
    return type->init_func;
}


bitmask_t *objtype_icon_mask (objtype_t *type)
{
    return type->icon_mask;
}
