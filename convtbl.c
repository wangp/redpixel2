/* convtbl.c: "conversion tables" */

#include <string.h>
#include <malloc.h>

#include "convtbl.h"


/* create_conv_table:
 * destroy_conv_table:
 *  Do what they are called.
 */
struct convtable *construct_convtable(unsigned int size)
{
    struct convtable *tbl;
    int i;
    
    tbl = malloc(sizeof(convtable_t));
    if (!tbl) 
      return NULL;
    
    tbl->tbl = malloc(sizeof(struct convitem) * size);
    if (!tbl) {
	free(tbl);
	return NULL;
    }

    tbl->size = size;
    for (i=0; i<size; i++)
      tbl->tbl[i].key = tbl->tbl[i].data = NULL;

    return tbl;  
}

void destroy_convtable(struct convtable *tbl, void (*freer)(void *))
{   
    int i;
    if (!tbl)
      return;
    
    for (i=0; i<tbl->size; i++) {
	if (tbl->tbl[i].key) free(tbl->tbl[i].key);
	if (tbl->tbl[i].data) freer(tbl->tbl[i].data);
    }
    
    free(tbl->tbl);
    free(tbl);
}


/* add_convtable_item:
 *  Adds a item to the table, returning NULL on error.
 */
convitem_t *add_convtable_item(convtable_t *tbl, char *key, void *data)
{
    int i;
    for (i=0; i<tbl->size; i++) {
	if (!tbl->tbl[i].key) {
	    tbl->tbl[i].key = strdup(key);
	    tbl->tbl[i].data = data;
	    return &tbl->tbl[i];
	}
    }
    return NULL;
}
