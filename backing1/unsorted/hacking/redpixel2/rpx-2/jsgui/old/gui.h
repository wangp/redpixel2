#ifndef _included_gui_h_
#define _included_gui_h_



typedef struct GUI_WM 
{
    char *name;
    int (*init)(int w, int h, int bpp);
    void (*shutdown)();
    int (*new_client)(JSObject *obj, int w, int h);
    void (*remove_client)(int id);
    void (*mark_client)(int id);
    void (*update)();
} GUI_WM;



void gui_set_window_manager(GUI_WM *newwm);
int gui_init(JSContext *cx, JSObject *glob, char *module_path, int w, int h, int bpp);
void gui_exit();
void gui_set_colours(int base, int hl, int sh);
void gui_set_text_colour(int textcol);
void gui_set_font(void *defaultfont);
void gui_set_display(void *p);

int gui_update();



#endif /* _included_gui_h_ */
