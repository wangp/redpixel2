/* Hash table functions, taken from Snippets.
 * Public domain code by Jerry Coffin, with improvements by HenkJan Wolthuis.
 * Hacked to pieces by Peter Wang, June 1999.
 */


#include <string.h>
#include <stdlib.h>

#include "wumpus.h"



/* hash_construct:
 *  Initialize the hash table to the size asked for.
 *  If it can't allocate sufficient memory, signals error by setting 
 *  the size of the table to 0.
 */
HASH_TABLE *hash_construct(HASH_TABLE *table, int size)
{
    int i;
    BUCKET **temp;

    table->size = size;
    table->table = (BUCKET **)malloc(sizeof(BUCKET *) * size);
    temp = table->table;

    if (temp == NULL) {
	table->size = 0;
	return table;
    }

    for (i = 0; i < size; i++)
      temp[i] = NULL;
    return table;
}



/* hash: 
 *  Hashes a string to an unsigned short (16 bits).
 */
static unsigned short hash(const char *string)
{
    unsigned short ret_val = 0;
    int i;

    while (*string) {
	i = *(int *)string;
	ret_val ^= i;
	ret_val <<= 1;
	string++;
    }
    
    return ret_val;
}



/* hash_insert:
 *  Insert 'key' into hash table.
 *  Returns pointer to old data associated with the key, if any, or
 *  NULL if the key wasn't in the table previously.
 */
void *hash_insert(const char *key, void *data, HASH_TABLE *table)
{
    unsigned int val = hash(key) % table->size;
    BUCKET *ptr;
    
    /* NULL means this bucket hasn't been used yet */
    if (NULL == (table->table)[val]) {
	(table->table)[val] = (BUCKET *)malloc(sizeof(BUCKET));
	if (NULL == (table->table)[val])
	  return NULL;

	(table->table)[val]->key = strdup(key);
	(table->table)[val]->next = NULL;
	(table->table)[val]->data = data;
	return (table->table)[val]->data;
    }

    /* This spot in the table is already in use.  See if the current string
     * has already been inserted, and if so, increment its count.
     */
    for (ptr = (table->table)[val]; NULL != ptr; ptr = ptr->next)
      if (0 == strcmp(key, ptr->key)) {
	  void *old_data;
	  
	  old_data = ptr->data;
	  ptr->data = data;
	  return old_data;
      }
    
    /* This key must not be in the table yet.  We'll add it to the head of
     * the list at this spot in the hash table.  Speed would be
     * slightly improved if the list was kept sorted instead.  In this case,
     * this code would be moved into the loop above, and the insertion would
     * take place as soon as it was determined that the present key in the
     * list was larger than this one.
     */

    ptr = (BUCKET *)malloc(sizeof(BUCKET));
    if (NULL == ptr)
      return 0;
    ptr->key = strdup(key);
    ptr->data = data;
    ptr->next = (table->table)[val];
    (table->table)[val] = ptr;
    return data;
}



/* hash_lookup:
 *  Look up a key and return the associated data.  
 *  Returns NULL if the key is not in the table.
 */
void *hash_lookup(const char *key, HASH_TABLE *table)
{
    unsigned int val = hash(key) % table->size;
    BUCKET *ptr;
    
    if (NULL == (table->table)[val])
      return NULL;

    for (ptr = (table->table)[val]; NULL != ptr; ptr = ptr->next) 
      if (0 == strcmp(key, ptr->key))
	return ptr->data;

    return NULL;
}



/* hash_del:
 *  Delete a key from the hash table and return associated
 *  data, or NULL if not present.
 */
void *hash_del(const char *key, HASH_TABLE *table)
{
    unsigned int val = hash(key) % table->size;
    void *data;
    BUCKET *ptr, *last = NULL;
    
    if (NULL == (table->table)[val])
      return NULL;
    
    for (last = NULL, ptr = (table->table)[val];
	 NULL != ptr;
	 last = ptr, ptr = ptr->next) {
	
	if (0 == strcmp(key, ptr->key)) {
	    if (last != NULL) {
		data = ptr->data;
		last->next = ptr->next;
		free(ptr->key);
		free(ptr);
		return data;
	    }
	    
	    /*
	     ** If 'last' still equals NULL, it means that we need to
	     ** delete the first node in the list. This simply consists
	     ** of putting our own 'next' pointer in the array holding
	     ** the head of the list.  We then dispose of the current
	     ** node as above.
	     */

	    else {
		data = ptr->data;
		(table->table)[val] = ptr->next;
		free(ptr->key);
		free(ptr);
		return data;
	    }
	}
    }

    return NULL;
}



/* hash_enumerate:
 *  Simply invokes the callback given as the second parameter for each
 *  node in the table, passing it the key and the associated data.
 */
void hash_enumerate(HASH_TABLE * table, void (*callback)(const char *, void *))
{
    int i;
    /*BUCKET *temp;*/

    for (i=0; i<table->size; i++) 
	if ((table->table)[i] != NULL) 
	    do {
		callback((table->table[i])->key, (table->table[i])->data);
	    } while (table->table[i]);
    
    /* PW: This next piece of DOSish code took me two days to track down.
     * (replaced by the `do ... while' above)
     */
#if 0    
          for (temp = (table->table)[i]; NULL != temp; temp = temp->next)
	      callback(temp->key, temp->data);
#endif    
}



/* These are used in freeing a table.  Perhaps I should code up
 * something a little less grungy, but it works, so what the heck.
 */
static void (*function)(void *) = (void (*)(void *))NULL;
static HASH_TABLE *the_table = NULL;



/* free_table iterates the table, calling this repeatedly to free
 * each individual node.  This, in turn, calls one or two other
 * functions - one to free the storage used for the key, the other
 * passes a pointer to the data back to a function defined by the user,
 * process the data as needed.
 */
static void free_node(const char *key, void *data)
{
    if (function)
	function(hash_del(key, the_table));
    else
	hash_del(key, the_table);
}



/* hash_free:
 *  Frees a complete table by iterating over it and freeing each node.
 *  The second parameter is the address of a function it will call with a
 *  pointer to the data associated with each node.  This function is
 *  responsible for freeing the data, or doing whatever is needed with
 *  it.
 */
void hash_free(HASH_TABLE *table, void (*func)(void *))
{
    function = func;
    the_table = table;

    hash_enumerate(table, free_node);
    free(table->table);
    table->table = NULL;
    table->size = 0;

    the_table = NULL;
    function = (void (*)(void *))NULL;
}
