/* weapon.c : weapons related */

#include <stdlib.h>
#include <string.h>
#include "weapon.h"


static int num = 0;

static weapon_type_t *head = NULL;


/* create_weapon_type:
 *  Adds a weapon to the internal list and returns the new type number.
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

int create_weapon_type(char *name) 
{
    weapon_type_t *newtype;
    newtype = malloc(sizeof(struct weapon_type));
    newtype->typenum = num;
    newtype->name = strdup(name);
    add(newtype);
    return num++;
}


/* assign_weapon_handler:
 *  Sets the handler function for TYPENUM.
 *  Returns zero if successful, non-zero on error.
 */
int assign_weapon_handler(int typenum, seer_addr handler)
{
    weapon_type_t *it = head;
    while (it) {
	if (it->typenum == typenum) {
	    it->handler = handler;
	    return 0;	
	}
	it = it->next;
    }
    return -1;
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
