/* object.c : object related stuff */

#include <lua.h>
#include "object.h"

static object_def_t *head = NULL;


/* register_object_type:
 *  Create a new object type and add it to internal list.
 */

static void add(object_def_t *addme)
{
    if (!head) {
	head = addme;
	addme->prev = addme->next = NULL;
    }
    else {
	object_def_t *it = head;
	while (it->next)
	  it = it->next;
	
	it->next = addme;
	addme->next = NULL;
	addme->prev = it;
    }
}

void register_object_type(char *name, lua_Object table, BITMAP *icon)
{
    object_def_t *obj = malloc(sizeof(object_def_t));

    obj->name = strdup(name);
    obj->table = table;
    obj->icon = icon;
    add(obj);
}


/* get_object_type:
 *  Return object_def struct corresponding to index I (not typenum).
 */
object_def_t *get_object_type(int i)
{
    object_def_t *it = head;
    while (it && i--)
      it = it->next;

    return it;
}


/* free_object_types:
 *  Free object types from memory.
 */

static void kill(object_def_t *obj)
{
    free(obj->name);
}

void free_object_types()
{
    object_def_t *it = head, *tmp;
    
    while (it) {
	tmp = it->next;
	kill(it);
	free(it);
	it = tmp;
    }
    
    head = NULL;
}


/* create_object:
 *  Malloc space for a new object.
 */
struct object *create_object(struct object_def *def)
{
    struct object *newobj = malloc(sizeof(struct object));
    
    if (newobj) 
      newobj->def = def;

    return newobj;
}


/* destroy_object:
 *  Free object from mem.
 */
void destroy_object(struct object *obj)
{
    if (obj)
      free(obj);
}
