/* jsgui.h - publically available API */

#ifndef __included_jsgui_h
#define __included_jsgui_h


typedef struct GUI_WM 
{
    char *name;
    int (*init)(int w, int h, int bpp);
    void (*shutdown)();
    int (*new_client)(int w, int h);
    void (*remove_client)(int id);
    void (*mark_client)(int id);
    void (*update)();
} GUI_WM;


void gui_set_jsgui_location(char *path);
void gui_set_window_manager(GUI_WM *newwm);

int gui_init(JSContext *cx, JSObject *glob,
	     int display_w, int display_h, int colour_depth);
void gui_exit();
void gui_set_colours(int base, int hl, int sh);
void gui_set_text_colour(int textcol);
void gui_set_display(void *p);

int gui_update();


#endif /* __included_jsgui_h */
