/* uglayout.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include "ug.h"


ug_dialog_layout_t *ug_dialog_layout_create ()
{
    ug_dialog_layout_t *p, end = { UG_DIALOG_LAYOUT_END };

    p = malloc (sizeof end);
    if (p)
	memcpy (p, &end, sizeof end);
    return p;
}


void ug_dialog_layout_destroy (ug_dialog_layout_t *p)
{
    free (p);
}


static int count (ug_dialog_layout_t *p)
{
    int n = 0;
    while ((int) p[n].class != -1)
	n++;
    return n + 1;
}


/* XXX: this could be done easier? */
ug_dialog_layout_t *ug_dialog_layout_insert (ug_dialog_layout_t *p,
					     ug_dialog_layout_t *newitem)
{
    ug_dialog_layout_t *q, end = { UG_DIALOG_LAYOUT_END };
    int num = count (p);
    int sz = sizeof (ug_dialog_layout_t);
    
    q = malloc (sz * (num + 1));

    memcpy (q, p, sz * (num - 1));	 /* leading up to terminator */
    memcpy (q + (num - 1), newitem, sz); /* insert at back */
    memcpy (q + num, &end, sz);		 /* insert new terminator */

    free (p);
    
    return q;
}
