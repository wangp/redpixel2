/* guiwm.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "gui.h"
#include "guiinter.h"


extern gui_wm_t gui_default_wm;

gui_wm_t *gui_wm = &gui_default_wm;


void gui_wm_set (gui_wm_t *wm)
{
    gui_wm = wm;
}

void gui_wm_init ()
{
    gui_wm->wm_init ();
}

void gui_wm_shutdown ()
{
    gui_wm->wm_shutdown ();
}

void gui_wm_event (gui_event_t event, int d)
{
    gui_wm->wm_event (event, d);
}

int gui_wm_update_screen (struct BITMAP *bmp)
{
    return gui_wm->wm_update_screen (bmp);
}


gui_window_t *gui_window_create (int x, int y, int w, int h, int flags)
{
    return gui_wm->window_create (x, y, w, h, flags);
}

void gui_window_destroy (gui_window_t *w)
{
    gui_wm->window_destroy (w);
}

void gui_window_move (gui_window_t *w, int x, int y)
{
    gui_wm->window_move (w, x, y);
}

void gui_window_move_relative (gui_window_t *w, int dx, int dy)
{
    gui_wm->window_move (w, gui_wm->window_x (w) + dx,
			    gui_wm->window_y (w) + dy);
}

void gui_window_resize (gui_window_t *win, int w, int h)
{
    gui_wm->window_resize (win, w, h);
}

void gui_window_lower (gui_window_t *w)
{
    gui_wm->window_lower (w);
}

void gui_window_raise (gui_window_t *w)
{
    gui_wm->window_raise (w);
}

void gui_window_show (gui_window_t *w)
{
    gui_wm->window_show (w);
}

void gui_window_hide (gui_window_t *w)
{
    gui_wm->window_hide (w);
}

void gui_window_dirty (gui_window_t *w)
{
    gui_wm->window_dirty (w);
}

void gui_window_set_title (gui_window_t *w, const char *title)
{
    gui_wm->window_set_title (w, title);
}

void gui_window_set_depth (gui_window_t *w, int depth)
{
    gui_wm->window_set_depth (w, depth);
}

void gui_window_set_alpha (gui_window_t *w, int alpha)
{
    gui_wm->window_set_alpha (w, alpha);
}

void gui_window_set_self (gui_window_t *w, void *self)
{
    gui_wm->window_set_self (w, self);
}

void gui_window_set_draw_proc (gui_window_t *w, void (*draw) (void *, struct BITMAP *))
{
    gui_wm->window_set_draw_proc (w, draw);
}

void gui_window_set_event_proc (gui_window_t *w, void (*event) (void *, gui_event_t, int))
{
    gui_wm->window_set_event_proc (w, event);
}

int gui_window_x (gui_window_t *w) { return gui_wm->window_x (w); }
int gui_window_y (gui_window_t *w) { return gui_wm->window_y (w); }
int gui_window_w (gui_window_t *w) { return gui_wm->window_w (w); }
int gui_window_h (gui_window_t *w) { return gui_wm->window_h (w); }
int gui_window_hidden (gui_window_t *w) { return gui_wm->window_hidden (w); }
