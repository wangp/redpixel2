/* ugbutton.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "ug.h"
#include "uginter.h"
#include "ugtheme.h"


struct button {
    char *label;
    int down;
};

#define private(x)	((struct button *) (x)->private)


static int button_create (ug_widget_t *p, void *label)
{
    p->private = malloc (sizeof (struct button));
    if (!p->private)
	return -1;
    memset (p->private, 0, sizeof (struct button));

    if (label)
	private (p)->label = ustrdup (label);

    return 0;
}


static void button_destroy (ug_widget_t *p)
{
    free (private (p)->label);
    free (private (p));
}


static void draw (ug_widget_t *p, BITMAP *bmp)
{
    int ofs = (private (p)->down) ? 1 : 0;

    ug_theme_tile (bmp, p->x + ofs, p->y + ofs, p->w - ofs, p->h - ofs, UG_THEME_FG);
    ug_theme_bevel (bmp, p->x, p->y, p->w, p->h, private (p)->down);

    if (private (p)->label)
	ug_theme_text_centre (bmp, p->x + ofs, p->y + ofs, p->w, p->h,
			      UG_THEME_FG, private (p)->label);
}


static void button_event (ug_widget_t *p, int event, void *d)
{
    switch (event) {
	case UG_EVENT_WIDGET_DRAW:
	    draw (p, ug_event_draw_bmp (d));
	    break;

	case UG_EVENT_MOUSE_DOWN:
	    if (ug_event_mouse_b (d) == 0) {
		private (p)->down = 1;
		ug_widget_dirty (p);
	    }
	    break;

	case UG_EVENT_MOUSE_UP:
	    if (ug_event_mouse_b (d) == 0) {
		private (p)->down = 0;
		ug_widget_dirty (p);

		ug_widget_emit_signal_mouse (p, event,
					     ug_event_mouse_x (d),
					     ug_event_mouse_y (d),
					     ug_event_mouse_b (d),
					     ug_event_mouse_bstate (d));
	    }
	    break;
    }
}


ug_widget_class_t ug_button = {
    button_create,
    button_destroy,
    button_event
};
