/* guiaccel.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include "gui.h"
#include "guiinter.h"


struct gui_accel {
    int key;
    void (*proc) ();
    void *param;
    struct gui_accel *next;
};


static gui_accel_t accels;


static void link_accel (gui_accel_t *p, gui_accel_t *q)
{
    q->next = p->next;
    p->next = q;
}

static void unlink_accel (gui_accel_t *p, gui_accel_t *q)
{
    while (p) {
	if (p->next == q) {
	    p->next = q->next;
	    break;
	}
	
	p = p->next;
    }
}


gui_accel_t *gui_accel_create (int key, void (*proc) ())
{
    gui_accel_t *p;

    if (!(p = malloc (sizeof *p)))
	return NULL;
    p->key = key;
    p->proc = proc;
    link_accel (&accels, p);
    return p;
}


void gui_accel_destroy (gui_accel_t *accel)
{
    if (accel) {
	unlink_accel (&accels, accel);
	free (accel);
    }
}


int gui_accel_handle_key (int key)
{
    gui_accel_t *p;

    for (p = accels.next; p; p = p->next)
	if ((key & 0xff) == p->key) {
	    p->proc ();
	    return 1;
	}

    return 0;
}
