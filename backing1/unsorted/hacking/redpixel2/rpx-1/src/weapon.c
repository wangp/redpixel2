/* weapon.c : weapons related */

#include <stdlib.h>
#include <string.h>
#include "weapon.h"


static weapon_type_t *head = NULL;


/* register_weapon_type:
 *  Adds a weapon to the internal list.
 */
static void add(weapon_type_t *addme)
{
    if (!head) {
	head = addme;
	addme->prev = addme->next = NULL;
    }
    else {
	weapon_type_t *it = head;
	while (it->next)
	  it = it->next;
	
	it->next = addme;
	addme->next = NULL;
	addme->prev = it;
    }
}

void register_weapon_type(char *name, lua_Object table)
{
    weapon_type_t *newtype;
    newtype = malloc(sizeof(struct weapon_type));
    newtype->name = strdup(name);
    newtype->table = table;
    add(newtype);
}


/* free_weapon_types:
 *  Free memory.
 */
static void kill(weapon_type_t *k)
{
    free(k->name);
}

void free_weapon_types()
{
    weapon_type_t *it = head, *tmp;
    
    while (it) {
	kill(it);
	tmp = it->next;
	free(it);
	it = tmp;
    }
    
    head = NULL;
}
