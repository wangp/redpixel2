#ifndef __included_gui_h
#define __included_gui_h


#include <allegro.h>


#define GUI_EVENT_KEY_DOWN		1
#define GUI_EVENT_KEY_UP		2
#define GUI_EVENT_KEY_HOLD		3
#define GUI_EVENT_KEY_TYPE		4

#define GUI_EVENT_MOUSE_MOVE		12
#define GUI_EVENT_MOUSE_DOWN		13
#define GUI_EVENT_MOUSE_UP		14
#define GUI_EVENT_MOUSE_WHEEL		15

#define GUI_EVENT_WINDOW_GOTMOUSE	21
#define GUI_EVENT_WINDOW_LOSTMOUSE	22
#define GUI_EVENT_WINDOW_FOCUSED	23
#define GUI_EVENT_WINDOW_UNFOCUSED	24
#define GUI_EVENT_WINDOW_RAISED		25
#define GUI_EVENT_WINDOW_LOWERED	26
#define GUI_EVENT_WINDOW_MOVED		27
#define GUI_EVENT_WINDOW_RESIZED	28
#define GUI_EVENT_WINDOW_DESTROYED	29


/* gui.c */

int gui_init ();
void gui_shutdown ();
void gui_main ();
void gui_quit ();


/* guiwm.c */

typedef struct gui_wm gui_wm_t;

void gui_wm_set (gui_wm_t *);


#define GUI_HINT_NOFRAME	1
#define GUI_HINT_STEALFOCUS	2

typedef struct gui_window gui_window_t;

gui_window_t *gui_window_create (int, int, int, int, int);
void gui_window_destroy (gui_window_t *);
void gui_window_move (gui_window_t *, int, int);
void gui_window_move_relative (gui_window_t *, int, int);
void gui_window_resize (gui_window_t *, int, int);
void gui_window_raise (gui_window_t *);
void gui_window_lower (gui_window_t *);
void gui_window_dirty (gui_window_t *);
void gui_window_set_title (gui_window_t *, const char *);
void gui_window_set_depth (gui_window_t *, int);
void gui_window_set_self (gui_window_t *, void *);
void gui_window_set_draw_proc (gui_window_t *, void (*) (void *, BITMAP *));
void gui_window_set_event_proc (gui_window_t *, void (*) (void *, int, int));

int gui_window_x (gui_window_t *);
int gui_window_y (gui_window_t *);
int gui_window_w (gui_window_t *);
int gui_window_h (gui_window_t *);


/* guimouse.c */

struct gui_mouse_state {
    int  x,  y,  z;
    int dx, dy, dz;
    
    struct {
	int down, up, state;
    } b[3];
};

extern struct gui_mouse_state gui_mouse;


#endif
