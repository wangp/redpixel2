/* datalist.c - list of datafiles loaded
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "wumpus.h"



#define GELLOPY_SIZE	400



typedef struct node {
    DATAFILE *data;
    struct node *next, *prev;
} node_t;



static node_t *list_head = NULL;



/* linked list code robbed from jsgui/guiwm.c */

static void insert(node_t *c)
{
    c->prev = NULL;
    c->next = list_head;
    if (list_head) 
	list_head->prev = c;
    list_head = c;
}


static void remove(node_t *c)
{
    if (c->next) c->next->prev = c->prev;
    if (c->prev) c->prev->next = c->next;
    if (c == list_head)
      list_head = c->next;
    c->next = c->prev = NULL;
}



/* rpx_load_data:
 *  Load datafile and insert into fat_gellopy.
 *  Returns node ptr on success, or NULL on failure.
 */
void *rpx_load_data(char *filename, char *branch)
{
    DATAFILE *d;
    node_t *n;
    
    d = load_datafile(filename);
    if (!d)
	return NULL;
    
    link_datafile(d, branch);
    
    n = malloc(sizeof(node_t));
    n->data = d;
    insert(n);
    
    return n;
}



/* rpx_unload_data:
 *  Unload datafile and unlink.
 */
void rpx_unload_data(void *ptr)
{
    node_t *n = ptr;
    
    if (!ptr)
	return;
    
    unload_datafile(n->data);
    unlink_datafile(n->data);
    remove(n);
    
    free(n);
}



/* rpx_data_cleanup:
 *  Initialise datafile stuff.
 */
void rpx_data_init()
{
    install_gellopy(GELLOPY_SIZE);
}



/* rpx_data_cleanup:
 *  Unload all datafiles.
 */
void rpx_data_cleanup()
{
    while (list_head) 
	rpx_unload_data(list_head);
}
