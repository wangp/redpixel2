/* alloc.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <string.h>
#include "alloc.h"


void *alloc (size_t size)
{
    void *p = malloc (size);
    if (p)
	memset (p, 0, size);
    return p;
}

