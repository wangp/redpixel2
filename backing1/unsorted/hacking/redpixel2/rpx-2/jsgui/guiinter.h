/* guiinter.h */

#ifndef __included_guiinter_h
#define __included_guiinter_h


#ifndef __included_gui_h
#error guiinter.h included without gui.h
#endif


/* Older (incomplete) versions actually *did* have switchable
 * window managers.  This features has been taken out (unneeded)
 * but the structure as you see below remains.
 */
struct gui_wm {
    char *name;
    
    int (*init)(int w, int h, int bpp);
    void (*shutdown)();
    
    int (*new_client)(JSObject *obj, int w, int h);
    void (*remove_client)(int id);
    void (*move_client)(int id, int rel_x, int rel_y);
    void (*mark_client)(int id);
    void (*raise_client)(int id);
    void (*lower_client)(int id);
    void (*client_special)(int id, int cmd);
    
    void (*update)();
    
    JSObject *(*focus_object)(int mx, int my);
    int (*focus_client)(int mx, int my);
};


/* globals */

extern JSScript  *__gui_script;
extern JSContext *__gui_cx;

extern struct gui_wm __gui_default_wm;
extern struct gui_wm *__gui_wm;

extern int __gui_width, __gui_height, __gui_bpp;
extern int __gui_base_colour, __gui_highlight_colour, __gui_shadow_colour;
extern int __gui_text_colour; 
extern FONT *__gui_font;
extern BITMAP *__gui_dest;

extern volatile int __gui_tics;


/* functions */

int __gui_draw_init_hook(JSContext *cx, JSObject *native);
void __gui_init_ticker();

JSBool JS_GetSubProperty(JSContext *cx, JSObject *obj, const char *name, jsval *vp);


#endif /* __included_guiinter_h */
