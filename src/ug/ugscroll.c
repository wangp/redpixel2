/* XXX - incomplete  */
#if 0

/* ugscroll.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "ug.h"
#include "uginter.h"
#include "ugtheme.h"


#define BUTTON_H	16
#define BUTTON_HH	(BUTTON_H * 2)


struct vscroll {
    int val, min, max;
    int step, bigstep;
};

#define PRIVATE(x)	((struct vscroll *) (x)->private)


static int vscroll_create (ug_widget_t *p, void *data)
{
    struct ug_vscroll_data *d;
    struct vscroll *pp;
    
    p->private = malloc (sizeof (struct vscroll));
    if (!p->private)
	return -1;
    memset (p->private, 0, sizeof (struct vscroll));

    if (data) {
	pp = PRIVATE (p);
	d = data;
	
	pp->val     = d->val;
	pp->min     = d->min;
	pp->max     = d->max;
	pp->step    = d->step;
	pp->bigstep = d->bigstep;
    }    
    
    return 0;
}


static void vscroll_destroy (ug_widget_t *p)
{
    free (p->private);
}


static void vscroll_draw (ug_widget_t *p, BITMAP *bmp)
{
    ug_theme_tile (bmp, p->x, p->y, p->w, p->h, UG_THEME_FG);

    /* tray */
    ug_theme_bevel (bmp, p->x, p->y, p->w, p->h, 1);

    /* handle */
    {
	int hh = p->h - BUTTON_HH;
	int yy;

	PRIVATE (p) -> min = 0;
	PRIVATE (p) -> val = 50;
	PRIVATE (p) -> max = 100;
	
	yy = PRIVATE (p)->val - PRIVATE (p)->min;
	yy *= hh;
	yy /= (PRIVATE (p)->max - PRIVATE (p)->min);
	yy -= hh / 8;

	ug_theme_bevel (bmp, (p->x + 2), (p->y + BUTTON_H) + yy, (p->w - 4), (hh / 4), 0);
    }

    /* arrows */
    ug_theme_arrow (bmp, (p->x + 2), (p->y + 2),               (p->w - 4), (BUTTON_H - 2), UG_THEME_ARROW_UP);
    ug_theme_arrow (bmp, (p->x + 2), (p->y + p->h - BUTTON_H), (p->w - 4), (BUTTON_H - 2), UG_THEME_ARROW_DOWN);
}


static int vscroll_event (ug_widget_t *p, int event, int d)
{
    return 0;
}


ug_widget_class_t ug_vscroll = {
    vscroll_create,
    vscroll_destroy,
    vscroll_draw,
    vscroll_event
};


#endif
