/* convtbl.c: "conversion tables" */

#include <string.h>
#include <malloc.h>
#include <allegro.h>		       /* for MAX() */

#include "convtbl.h"


/* create_conv_table:
 * destroy_conv_table:
 *  Do what they are called.
 */
struct convtable *construct_convtable()
{
    struct convtable *tbl;
    
    tbl = malloc(sizeof(convtable_t));
    if (!tbl) 
      return NULL;

    tbl->size = 0;
    tbl->tbl = NULL;

    return tbl;  
}

static void _dummy_freer(void *p) {}

void destroy_convtable(struct convtable *tbl, void (*freer)(void *))
{   
    int i;
    if (!tbl)
      return;
    
    if (!freer)
      freer = _dummy_freer;
    
    for (i=0; i<tbl->size; i++) {
	if (tbl->tbl[i].key) free(tbl->tbl[i].key);
	if (tbl->tbl[i].data) freer(tbl->tbl[i].data);
    }
    
    if (tbl->tbl) free(tbl->tbl);
    free(tbl);
}


/* add_convtable_item:
 *  Adds a item to the table.
 */
void add_convtable_item(convtable_t *tbl, char *key, void *data)
{
    int i = tbl->size;
    
    if (tbl->tbl)
      tbl->tbl = realloc(tbl->tbl, sizeof(struct convitem) * (i+1));
    else
      tbl->tbl = malloc(sizeof(struct convitem));

    tbl->size = i+1;
    tbl->tbl[i].key = strdup(key);
    tbl->tbl[i].data = data;
}


/* translate_convtable:
 *  Translate an index from one convtable to matching index on other c-tbl.
 */
int translate_convtable(struct convtable *dest, struct convtable *src, int i)
{
    char *match;
    int j, max;
    
    max = MIN(src->size, dest->size);
    match = src->tbl[i].key;
    
    for (j=0; j<max; j++)
      if (strcmp(match, dest->tbl[j].key) == 0)
	return j;

    return -1;
}
