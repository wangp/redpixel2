#ifndef __included_ug_h
#define __included_ug_h


#include "gui.h"


#define UG_EVENT_KEY_DOWN		1
#define UG_EVENT_KEY_UP			2
#define UG_EVENT_KEY_TYPE		3

#define UG_EVENT_MOUSE_MOVE		12
#define UG_EVENT_MOUSE_DOWN		13
#define UG_EVENT_MOUSE_UP		14
#define UG_EVENT_MOUSE_WHEEL		15

#define UG_EVENT_WIDGET_GOTMOUSE	21
#define UG_EVENT_WIDGET_LOSTMOUSE	22
#define UG_EVENT_WIDGET_FOCUSED		23
#define UG_EVENT_WIDGET_UNFOCUSED	24
#define UG_EVENT_WIDGET_DRAW		99


/* widget classes */

typedef struct ug_widget_class ug_widget_class_t;

extern ug_widget_class_t ug_blank;
extern ug_widget_class_t ug_button;


/* ugwidget.c */

typedef struct ug_widget ug_widget_t;

ug_widget_t *ug_widget_create (ug_widget_class_t *, void *, const char *);
void ug_widget_destroy (ug_widget_t *);
void ug_widget_dirty (ug_widget_t *);
void ug_widget_focus (ug_widget_t *);

int ug_widget_x (ug_widget_t *);
int ug_widget_y (ug_widget_t *);
int ug_widget_w (ug_widget_t *);
int ug_widget_h (ug_widget_t *);
char *ug_widget_id (ug_widget_t *);


/* ugevent.c */

void ug_widget_send_event (ug_widget_t *w, int event);
void ug_widget_emit_signal (ug_widget_t *w, int event);

void ug_widget_send_event_key (ug_widget_t *w, int event, int key);
void ug_widget_emit_signal_key (ug_widget_t *w, int event, int key);
int ug_event_key (void *);

void ug_widget_send_event_mouse (ug_widget_t *w, int event, int x, int y, int b, int bstate);
void ug_widget_emit_signal_mouse (ug_widget_t *w, int event, int x, int y, int b, int bstate);
int ug_event_mouse_x (void *);
int ug_event_mouse_y (void *);
int ug_event_mouse_rel_x (void *);
int ug_event_mouse_rel_y (void *);
int ug_event_mouse_b (void *);
int ug_event_mouse_bstate (void *);

void ug_widget_send_event_draw (ug_widget_t *w, int event, BITMAP *bmp);
void ug_widget_emit_signal_draw (ug_widget_t *w, int event, BITMAP *bmp);
BITMAP *ug_event_draw_bmp (void *);
int ug_event_draw_x (void *);
int ug_event_draw_y (void *);
int ug_event_draw_w (void *);
int ug_event_draw_h (void *);


/* uglayout.c */

typedef struct ug_dialog_layout {
    ug_widget_class_t *class;
    int w, h;
    void *data;
    void (*slot) (ug_widget_t *, int, void *);
    const char *id;
} ug_dialog_layout_t;

#define UG_DIALOG_LAYOUT_BR	((ug_widget_class_t *)  0), 0, 0, 0, 0, 0
#define UG_DIALOG_LAYOUT_END	((ug_widget_class_t *) -1), 0, 0, 0, 0, 0

ug_dialog_layout_t *ug_dialog_layout_create ();
void ug_dialog_layout_destroy (ug_dialog_layout_t *);
ug_dialog_layout_t *ug_dialog_layout_insert (ug_dialog_layout_t *p,
					     ug_dialog_layout_t *newitem);


/* ugdialog.c */

typedef struct ug_dialog ug_dialog_t;

ug_dialog_t *ug_dialog_create (gui_window_t *, ug_dialog_layout_t *, int);
void ug_dialog_destroy (ug_dialog_t *);
void ug_dialog_dirty (ug_dialog_t *);
void ug_dialog_focus (ug_dialog_t *, ug_widget_t *);

int ug_dialog_x (ug_dialog_t *);
int ug_dialog_y (ug_dialog_t *);
int ug_dialog_w (ug_dialog_t *);
int ug_dialog_h (ug_dialog_t *);
ug_widget_t *ug_dialog_widget (ug_dialog_t *, char *id);


/* ug.c */

int ug_init ();
void ug_shutdown ();


#endif
