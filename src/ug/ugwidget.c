/* ugwidget.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <string.h>
#include "ug.h"
#include "uginter.h"


ug_widget_t *ug_widget_create (ug_widget_class_t *class, void *data, const char *id)
{
    ug_widget_t *p;

    p = malloc (sizeof *p);
    if (!p) return 0;
    memset (p, 0, sizeof *p);

    p->class = class;
	
    if (class->create && class->create (p, data) < 0) {
	free (p);
	return 0;
    }

    if (id)
	p->id = ustrdup (id);
    
    return p;
}


void ug_widget_destroy (ug_widget_t *p)
{
    if (p) {
	if (p->class->destroy)
	    p->class->destroy (p);
	free (p->id);
	free (p);
    }
}


void ug_widget_dirty (ug_widget_t *p)
{
    if (p->dialog)
	ug_dialog_dirty (p->dialog);
}


void ug_widget_focus (ug_widget_t *p)
{
    if (p->dialog)
	ug_dialog_focus (p->dialog, p);
}


int ug_widget_x (ug_widget_t *w)
{
    return w->x + ug_dialog_x (w->dialog);
}

int ug_widget_y (ug_widget_t *w)
{
    return w->y + ug_dialog_y (w->dialog);
}

int ug_widget_w (ug_widget_t *w) { return w->w; }
int ug_widget_h (ug_widget_t *w) { return w->h; }
char *ug_widget_id (ug_widget_t *w) { return w->id; }
