/* ugevent.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "ug.h"
#include "uginter.h"


#define MAKE_GETTER(RET, NAME, EVTYPE, MEMB)	\
	RET NAME (void *p) { return ((struct EVTYPE *) p)->MEMB; }


static void do_event (ug_widget_t *w, int event, void *data)
{
    if (w && w->class->event) w->class->event (w, event, data);
}

static void do_signal (ug_widget_t *w, int event, void *data)
{
    if (w && w->slot) w->slot (w, event, data);
}


/*----------------------------------------------------------------------*/

void ug_widget_send_event (ug_widget_t *w, int event)
{
    do_event (w, event, 0);
}

void ug_widget_emit_signal (ug_widget_t *w, int event)
{
    do_signal (w, event, 0);
}


/*----------------------------------------------------------------------*/

void ug_widget_send_event_ex (ug_widget_t *w, int event, void *d)
{
    do_event (w, event, d);
}

void ug_widget_emit_signal_ex (ug_widget_t *w, int event, void *d)
{
    do_signal (w, event, d);
}


/*----------------------------------------------------------------------*/

struct key {
    int key;
};

void ug_widget_send_event_key (ug_widget_t *w, int event, int key)
{
    struct key data = { key };
    do_event (w, event, &data);
}

void ug_widget_emit_signal_key (ug_widget_t *w, int event, int key)
{
    struct key data = { key };
    do_signal (w, event, &data);
}

MAKE_GETTER(int, ug_event_key, key, key)


/*----------------------------------------------------------------------*/

struct mouse {
    int x, y;
    int rel_x, rel_y;
    int b, bstate;
};

void ug_widget_send_event_mouse (ug_widget_t *w, int event, int x, int y, int b, int bstate)
{
    struct mouse data = { x, y,
			  x - ug_widget_x (w),
			  y - ug_widget_y (w),
			  b, bstate };
    do_event (w, event, &data);
}

void ug_widget_emit_signal_mouse (ug_widget_t *w, int event, int x, int y, int b, int bstate)
{
    struct mouse data = { x, y,
			  x - ug_widget_x (w),
			  y - ug_widget_y (w),
			  b, bstate };
    do_signal (w, event, &data);
}

MAKE_GETTER(int, ug_event_mouse_x, 	mouse, x)
MAKE_GETTER(int, ug_event_mouse_y, 	mouse, y)
MAKE_GETTER(int, ug_event_mouse_rel_x, 	mouse, rel_x)
MAKE_GETTER(int, ug_event_mouse_rel_y, 	mouse, rel_y)
MAKE_GETTER(int, ug_event_mouse_b, 	mouse, b)
MAKE_GETTER(int, ug_event_mouse_bstate, mouse, bstate)


/*----------------------------------------------------------------------*/

struct draw {
    BITMAP *bmp;
    int x, y, w, h;
};

void ug_widget_send_event_draw (ug_widget_t *w, int event, BITMAP *bmp)
{
    struct draw data = { bmp, w->x, w->y, w->w, w->h };	
    do_event (w, event, &data);
}

void ug_widget_emit_signal_draw (ug_widget_t *w, int event, BITMAP *bmp)
{
    struct draw data = { bmp, w->x, w->y, w->w, w->h };	
    do_signal (w, event, &data);
}

MAKE_GETTER(BITMAP *, ug_event_draw_bmp, draw, bmp)
MAKE_GETTER(int, ug_event_draw_x, draw, x)
MAKE_GETTER(int, ug_event_draw_y, draw, y)
MAKE_GETTER(int, ug_event_draw_w, draw, w)
MAKE_GETTER(int, ug_event_draw_h, draw, h)
