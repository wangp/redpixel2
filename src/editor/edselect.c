/* edselect.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <string.h>
#include <allegro.h>
#include "ug.h"
#include "uginter.h"
#include "alloc.h"
#include "edselect.h"
#include "magic4x4.h"
#include "rect.h"


typedef struct {
    char *name;
    BITMAP *bmp;
} item_t;

struct ed_select_list {
    int num;
    item_t *item;
};

typedef ed_select_list_t list_t;


list_t *ed_select_list_create (void)
{
    return alloc (sizeof (list_t));
}

void ed_select_list_destroy (list_t *p)
{
    int i;

    if (!p) return;

    for (i = 0; i < p->num; i++)
	free (p->item[i].name);
    free (p->item);
    free (p);
}

void ed_select_list_add_item (list_t *p, const char *name, BITMAP *bmp)
{
    item_t *i;

    p->item = realloc (p->item, sizeof (item_t) * (p->num + 1));
    i = p->item + p->num;
    i->name = ustrdup (name);
    i->bmp = bmp;

    p->num++;
}

char *ed_select_list_item_name (list_t *p, int i)
{
    if (i < p->num)
	return p->item[i].name;
    else
	return 0;
}

int ed_select_list_item_index (list_t *p, const char *name)
{
    int i;

    for (i = 0; i < p->num; i++)
	if (!ustrcmp (p->item[i].name, name))
	    return i;

    return -1;
}


/*----------------------------------------------------------------------*/

static int pink;

struct sel {
    list_t *list;
    int icon_w, icon_h;
    int rows, cols;
    int top, selected;
};

#define private(x)	((struct sel *) (x)->private)


static int sel_create (ug_widget_t *p, void *label)
{
    p->private = alloc (sizeof (struct sel));
    pink = makecol (0xc0, 0xa0, 0xa0);
    return 0;
}

static void sel_destroy (ug_widget_t *p)
{
    free (private (p));
}


/*----------------------------------------------------------------------*/


#define PAGE(s)		(((struct sel *) s)->cols * ((struct sel *) s)->rows)
#define STEP(s)		(PAGE (s) / 4)


static void sel_scroll_up (struct sel *s, int count)
{
    if (!s->list) return;

    while (count > 0) {
	if (s->top <= 0)
	    break;
	s->top -= s->cols;
	count -= s->cols;
    }

    if (s->top < 0)
	s->top = 0;
}

static void sel_scroll_down (struct sel *s, int count)
{
    if (!s->list) return;
    
    while (count > 0) {
	if (s->top + PAGE(s) >= s->list->num)
	    break;
	s->top += s->cols;
	count -= s->cols;
    }
}

static void sel_scroll_until_selected_in_view (struct sel *s)
{
    while (s->selected < s->top)
	sel_scroll_up (s, 1);
    while (s->selected > (s->top + (s->rows * s->cols) - 1))
	sel_scroll_down (s, 1);
}


static void draw_magic_max_stretch (BITMAP *dest, BITMAP *src,
				    int x, int y, int w, int h)
{
    BITMAP *tmp;
    float ww, hh;

    tmp = unget_magic_bitmap_format (src);
	
    if (tmp->w > tmp->h) {
	ww = 1.0;
	hh = (float) tmp->h / (float) tmp->w;
    }
    else {
	ww = (float) tmp->w / (float) tmp->h;
	hh = 1.0;
    }

    stretch_sprite (dest, tmp, x, y, ww * w, hh * h);
    destroy_bitmap (tmp);
}

static void dottify (BITMAP *b, int x1, int y1, int x2, int y2, int c)
{
    int x;
    for (; y1 <= y2; y1++)
	for (x = x1 + (y1 % 6); x <= x2; x += 6)
	    putpixel (b, x, y1, c);
}    

static void sel_draw (struct sel *s, BITMAP *bmp, int x, int y, int w, int h)
{
    int xx, yy, i;
    item_t *p;

    if (!s->list)
	return;

    for (i = s->top, xx = x, yy = y;
	 (i < s->list->num) && (yy + s->icon_h - 1 < y + h);
	 i++) {
	p = s->list->item + i;

	draw_magic_max_stretch (bmp, p->bmp, xx, yy, s->icon_w, s->icon_h);

	if (i == s->selected)
	    dottify (bmp, xx, yy, xx + s->icon_w - 1, yy + s->icon_h - 1, pink);
	
	if (xx + s->icon_w >= x + w)
	    xx = 0, yy += s->icon_h;
	else
	    xx += s->icon_w;
    }
}


static void sel_emit_selected (ug_widget_t *widget)
{
    ug_widget_emit_signal (widget, ED_SELECT_SIGNAL_SELECTED, 0);
}

static int sel_select (ug_widget_t *widget, int mx, int my)
{
    struct sel *p = widget->private;
    int u, v, i;

    if (!p->list) return 0;

    for (v = 0, i = p->top; v < p->rows; v++)
	for (u = 0; (u < p->cols) && (i < p->list->num); u++, i++)
	    if (in_rect (mx, my, (u * p->icon_w), (v * p->icon_h),
			 p->icon_w, p->icon_h)) {
		if (p->selected == i)
		    return 0;
		
		p->selected = i;
		sel_emit_selected (widget);
		return 1;
	    }

    return 0;
}

static void sel_event (ug_widget_t *p, ug_event_t event, ug_event_data_t *d)
{
    switch (event) {
	case UG_EVENT_WIDGET_DRAW:
	    sel_draw (p->private,
		      ug_event_data_draw_bmp (d),
		      ug_event_data_draw_x (d), ug_event_data_draw_y (d),
		      ug_event_data_draw_w (d), ug_event_data_draw_h (d));
	    break;

	case UG_EVENT_WIDGET_GOTMOUSE:
	    ug_widget_focus (p);
	    break;

	case UG_EVENT_MOUSE_MOVE:
	    if (!ug_event_data_mouse_bstate (d))
		break;
	    /* fall through */
 
	case UG_EVENT_MOUSE_DOWN:
        select:
	    if (sel_select (p, ug_event_data_mouse_rel_x (d),
			    ug_event_data_mouse_rel_y (d)))
		ug_widget_dirty (p);
	    break;

	case UG_EVENT_MOUSE_WHEEL:
	    if (ug_event_data_mouse_b (d) > 0)
		sel_scroll_up (p->private, MAX (STEP (p->private), 1));
	    else
		sel_scroll_down (p->private, MAX (STEP (p->private), 1));
	    ug_widget_dirty (p);

	    if (ug_event_data_mouse_bstate (d))
		goto select;
	    break;

	default:
	    break;
    }
}

ug_widget_class_t ed_select = {
    sel_create,
    sel_destroy,
    sel_event
};


/*----------------------------------------------------------------------*/

static void calc (ug_widget_t *w)
{
    struct sel *p = private (w);

    if (!p->list || !p->icon_w || !p->icon_h)
	return;

    p->cols = ug_widget_w (w) / p->icon_w;
    p->rows = ug_widget_h (w) / p->icon_h;
}

void ed_select_set_list (ug_widget_t *p, list_t *list)
{
    private (p)->list = list;
    calc (p);
    ug_widget_dirty (p);
}

void ed_select_set_icon_size (ug_widget_t *p, int w, int h)
{
    private (p)->icon_w = w;
    private (p)->icon_h = h;
    calc (p);
    ug_widget_dirty (p);
}

/* The scroll functions are for interactive use.  */

void ed_select_scroll_up (ug_widget_t *p)
{
    struct sel *s = private (p);
    if (s->selected > 0) {
	s->selected--;
	sel_scroll_until_selected_in_view (s);
	sel_emit_selected (p);
	ug_widget_dirty (p);
    }
}

void ed_select_scroll_down (ug_widget_t *p)
{
    struct sel *s = private (p);
    if (s->selected < s->list->num - 1) {
	s->selected++;
	sel_scroll_until_selected_in_view (s);
	sel_emit_selected (p);
	ug_widget_dirty (p);
    }
}

void ed_select_scroll_page_up (ug_widget_t *p)
{
    sel_scroll_up (p->private, PAGE (p->private) / 2);
    ug_widget_dirty (p);
}

void ed_select_scroll_page_down (ug_widget_t *p)
{
    sel_scroll_down (p->private, PAGE (p->private) / 2);
    ug_widget_dirty (p);
}

/* The set functions are for saving and restore state.  */

void ed_select_set_top (ug_widget_t *p, int top)
{
    private (p)->top = top;
}

int ed_select_top (ug_widget_t *p)
{
    return private (p)->top;
}

void ed_select_set_selected (ug_widget_t *p, int selected)
{
    private (p)->selected = selected;
}

int ed_select_selected (ug_widget_t *p)
{
    return private (p)->selected;
}
