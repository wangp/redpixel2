#ifndef _included_guiinter_h_
#define _included_guiinter_h_

#ifndef _included_gui_h_
#error guiinter.h included without gui.h
#endif


extern GUI_WM *__gui_wm;

extern GUI_WM __gui_default_wm;

extern int __gui_width, __gui_height, __gui_bpp;
extern int __gui_base_colour, __gui_highlight_colour, __gui_shadow_colour;
extern int __gui_text_colour; 
extern void *__gui_font;
extern void *__gui_dest;

extern JSScript  *__gui_script;
extern JSContext *__gui_cx;


int __gui_spew(JSObject *obj);


/* FIXME: debugging only */
#define shout(x) {alert(x, "", "", "Ok", NULL, 13, 17);}


#endif /* _included_guiinter_h_ */
