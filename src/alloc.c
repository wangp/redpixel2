/* alloc.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
#include "alloc.h"
#include "error.h"


void *alloc (size_t size)
{
    void *p = malloc (size);
    if (!p) error ("Out of memory.\n");
    memset (p, 0, size);
    return p;
}
