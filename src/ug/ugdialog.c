/* ugdialog.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "uginter.h"
#include "ugtheme.h"


static void dialog_draw (void *p, BITMAP *bmp);
static void dialog_event (void *p, int event, int data);


#define is_real_widget(x)	((int) (x) > 0)


static ug_dialog_t *create_dialog ()
{
    ug_dialog_t *d = malloc (sizeof *d);
    if (d) memset (d, 0, sizeof *d);
    return d;
}


static int add_widget (ug_dialog_t *d, ug_widget_t *w)
{
    void *p = realloc (d->widget, sizeof w * (d->num + 1));
    
    if (p) {
	d->widget = p;
        d->widget[d->num++] = w;
    }

    return (p) ? 0 : -1;
}


static void attach_window (ug_dialog_t *d, gui_window_t *w)
{
    gui_window_set_draw_proc (w, dialog_draw);
    gui_window_set_event_proc (w, dialog_event);
    gui_window_set_self (w, d);
    d->window = w;
}


static void detach_window (ug_dialog_t *d)
{
    gui_window_t *w = d->window;
    if (w) {
        gui_window_set_draw_proc (w, 0);
	gui_window_set_event_proc (w, 0);
	gui_window_set_self (w, 0);
	d->window = 0;
    }
}


static void layout_dialog (ug_dialog_t *d)
{
    int *row_shared_w, *row_fixed_h, total_remain_h;
    int i, j, rows;
    int x, y, ystep;
    ug_widget_t *p;

#define border		(d->border)
#define width		(gui_window_w (d->window))
#define height		(gui_window_h (d->window))
#define percent(x, y)	((x) * (y) / 100)

    /* pass 1: count number of rows */
    for (i = 0, rows = 1; i < d->num; i++)
	if (!d->widget[i]) rows++;

    row_shared_w = malloc (sizeof (int) * rows);
    row_fixed_h  = malloc (sizeof (int) * rows);

    /* pass 2: fill in row data */
    row_shared_w[0] = width - border;
    row_fixed_h[0] = 0;
	    
    for (i = j = 0; i < d->num; i++) {
	p = d->widget[i];
	
	if (!p) {
	    j++;
	    row_shared_w[j] = width - border;
	    row_fixed_h[j] = 0;
	}
	else {
	    if (p->rw > 0)
		row_shared_w[j] -= p->rw;
	    row_shared_w[j] -= border;
	    
	    if (p->rh > 0)
		row_fixed_h[j] = MAX (row_fixed_h[j], p->rh);
	}
    }

    j = total_remain_h = height - 2 * border;
    
    for (i = 0; i < rows; i++)
	if (row_fixed_h[i])
	    total_remain_h -= row_fixed_h[i] + border;

    /* special case: no fixed heights */
    if (total_remain_h == j) {
	total_remain_h -= border;
	total_remain_h /= rows;
    }
    
    /* pass 3: allocate widget sizes and positions */
    x = y = border;
    ystep = 0;
        
    for (i = j = 0; i < d->num; i++) {
	p = d->widget[i];
	
	if (!p) {
	    x = border;
	    y += ystep + border;
	    ystep = 0;
	    
	    j++;
	    continue;
	}
	
	p->x = x;
	p->y = y;

	if (p->rw > 0)
	    p->w = p->rw;
	else
	    p->w = percent (-p->rw, row_shared_w[j]);

	if (p->rh > 0)
	    p->h = p->rh;
	else
	    p->h = percent (-p->rh, (row_fixed_h[j]
				     ? row_fixed_h[j]
				     : total_remain_h));

	x += p->w + border;
	ystep = MAX (ystep, p->h);
    }

    free (row_shared_w);
    free (row_fixed_h);

#undef border width height percent
}


ug_dialog_t *ug_dialog_create (gui_window_t *win, ug_dialog_layout_t *layout, int border)
{
    ug_dialog_t *d;
    ug_widget_t *w;
    ug_dialog_layout_t *l;
    int i;

    d = create_dialog ();
    if (!d) goto error;

    for (i = 0; (layout[i].class != (ug_widget_class_t *) -1); i++) {
	l = layout + i;
  
	if (l->class) {
	    w = ug_widget_create (l->class, l->data, l->id);
	    w->dialog = d;
	    w->rw = l->w;
	    w->rh = l->h;
	    w->slot = l->slot;
	}
	else {
	    w = 0;	/* UG_DIALOG_LAYOUT_BR */
	}

	if (add_widget (d, w) < 0)
	    goto error;
    }

    attach_window (d, win);

    d->border = border;
    layout_dialog (d);

    return d;

  error:

    if (d) ug_dialog_destroy (d);
    return 0;
}


void ug_dialog_destroy (ug_dialog_t *d)
{
    if (d) {
	detach_window (d);
	free (d->widget);
	free (d);
    }
}


/*----------------------------------------------------------------------*/

static void dialog_draw (void *p, BITMAP *bmp)
{
    ug_dialog_t *d = p;
    int i;

    ug_theme_tile (bmp, 0, 0, bmp->w, bmp->h, UG_THEME_BG);

    for (i = 0; i < d->num; i++)
	if (is_real_widget (d->widget[i]))
	    ug_widget_send_event_draw (d->widget[i], UG_EVENT_WIDGET_DRAW, bmp);
}


static ug_widget_t *find_widget (ug_dialog_t *d, int x, int y)
{
    ug_widget_t *w, *z = 0;
    int i;
    
    for (i = 0; i < d->num; i++) {
	w = d->widget[i];
	if (w && ((x >= w->x) && (y >= w->y) &&
		  (x < w->x + w->w) && (y < w->y + w->h)))
	    z = w;
    }

    return z;
}


static void do_mouse_event (ug_widget_t *w, int event, int b)
{
    if (w)
	ug_widget_send_event_mouse (w, event, 
				    gui_mouse.x, gui_mouse.y, b,
				    ((gui_mouse.b[0].state)
				     | (gui_mouse.b[1].state << 1)
				     | (gui_mouse.b[2].state << 2)));
}


static void dialog_event (void *p, int event, int data)
{
    ug_dialog_t *d = p;

    switch (event) {
	case GUI_EVENT_KEY_TYPE:
	    ug_widget_send_event_key (d->focus, UG_EVENT_KEY_TYPE, data);
	    break;
	    
	case GUI_EVENT_KEY_DOWN:
	    ug_widget_send_event_key (d->focus, UG_EVENT_KEY_DOWN, data);
	    break;
	    
	case GUI_EVENT_KEY_UP:
	    ug_widget_send_event_key (d->focus, UG_EVENT_KEY_UP, data);
	    break;

	case GUI_EVENT_MOUSE_MOVE: {
	    ug_widget_t *w = find_widget (d, gui_mouse.x - gui_window_x (d->window),
					     gui_mouse.y - gui_window_y (d->window));
	    if (d->hasmouse != w) {
		ug_widget_send_event (d->hasmouse, UG_EVENT_WIDGET_LOSTMOUSE);
		
		d->hasmouse = w;
		ug_widget_send_event (d->hasmouse, UG_EVENT_WIDGET_GOTMOUSE);
	    }

	    do_mouse_event (d->focus, UG_EVENT_MOUSE_MOVE, data);
	    break;
	}

	case GUI_EVENT_MOUSE_DOWN: {
	    ug_widget_t *w = find_widget (d, gui_mouse.x - gui_window_x (d->window),
					     gui_mouse.y - gui_window_y (d->window));
	    ug_dialog_focus (d, w);
	    do_mouse_event (d->focus, UG_EVENT_MOUSE_DOWN, data);
	    break;
	}

	case GUI_EVENT_MOUSE_UP:
	    do_mouse_event (d->focus, UG_EVENT_MOUSE_UP, data);
	    break;
	    
	case GUI_EVENT_MOUSE_WHEEL:
	    do_mouse_event (d->focus, UG_EVENT_MOUSE_WHEEL, data);
	    break;

	case GUI_EVENT_WINDOW_LOSTMOUSE: {
	    /* XXX: not good behavior */ 
	    int i; for (i = 0; i < 3; i++)
		if (gui_mouse.b[i].state)
		    do_mouse_event (d->focus, UG_EVENT_MOUSE_UP, i);
	    break;
	}
	    
	case GUI_EVENT_WINDOW_RESIZED:
	    layout_dialog (d);
	    break;

	case GUI_EVENT_WINDOW_DESTROYED: 
	    ug_dialog_destroy (d);
	    break;

	default:
	    break;
    }
}


void ug_dialog_dirty (ug_dialog_t *d)
{
    if (d->window)
	gui_window_dirty (d->window);
}


void ug_dialog_focus (ug_dialog_t *d, ug_widget_t *w)
{
    if (d->focus != w) {
	ug_widget_send_event (d->focus, UG_EVENT_WIDGET_UNFOCUSED);
		
	d->focus = w;
	ug_widget_send_event (d->focus, UG_EVENT_WIDGET_FOCUSED);
    }
}


int ug_dialog_x (ug_dialog_t *d) { return gui_window_x (d->window); }
int ug_dialog_y (ug_dialog_t *d) { return gui_window_y (d->window); }
int ug_dialog_w (ug_dialog_t *d) { return gui_window_w (d->window); }
int ug_dialog_h (ug_dialog_t *d) { return gui_window_h (d->window); }

ug_widget_t *ug_dialog_widget (ug_dialog_t *d, char *id)
{
    int i;
    
    for (i = 0; i < d->num; i++)
	if (is_real_widget (d->widget[i])
	    && (d->widget[i]->id)
	    && (!ustrcmp (d->widget[i]->id, id)))
	    return d->widget[i];

    return 0;
}
