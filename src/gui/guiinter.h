#ifndef __included_guiinter_h
#define __included_guiinter_h


/* guiaccel.c */

int gui_accel_handle_key (int key);


/* guidirty.c */

void gui_dirty_init ();
void gui_dirty_shutdown ();
void gui_dirty_add_rect (int x, int y, int w, int h);
void gui_dirty_clear (BITMAP *bmp);
void gui_dirty_blit (BITMAP *bmp, BITMAP *dest);


/* guimouse.c */

void gui_mouse_update ();


/* guiwm.c */

struct gui_wm {
    void (*wm_init) ();
    void (*wm_shutdown) ();
    void (*wm_event) (int, int);
    int (*wm_update_screen) (BITMAP *);
    
    gui_window_t *(*window_create) (int, int, int, int, int);
    void (*window_destroy) (gui_window_t *);
    void (*window_move) (gui_window_t *, int, int);
    void (*window_resize) (gui_window_t *, int, int);
    void (*window_lower) (gui_window_t *);
    void (*window_raise) (gui_window_t *);
    void (*window_dirty) (gui_window_t *);
    void (*window_set_title) (gui_window_t *, const char *);
    void (*window_set_depth) (gui_window_t *, int);
    void (*window_set_self) (gui_window_t *, void *);
    void (*window_set_draw_proc)  (gui_window_t *, void (*) (void *, BITMAP *));
    void (*window_set_event_proc) (gui_window_t *, void (*) (void *, int, int));

    int (*window_x) (gui_window_t *);
    int (*window_y) (gui_window_t *);
    int (*window_w) (gui_window_t *);
    int (*window_h) (gui_window_t *);
};

extern gui_wm_t *gui_wm;

void gui_wm_init ();
void gui_wm_shutdown ();
void gui_wm_event (int event, int d);
int gui_wm_update_screen (BITMAP *);


#endif
