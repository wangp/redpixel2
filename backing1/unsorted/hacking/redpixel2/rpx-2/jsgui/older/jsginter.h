/* jsginter.h -- internal stuff */

#ifndef __included_jsginter_h
#define __included_jsginter_h

#ifndef __included_jsgui_h
#error Error in include order!
#endif


#define GUI_ALIGN_LEFT		0
#define GUI_ALIGN_TOP		0
#define GUI_ALIGN_CENTRE	1
#define GUI_ALIGN_RIGHT		2
#define GUI_ALIGN_BOTTOM	2


extern int __gui_width, __gui_height;
extern int __gui_base_colour, __gui_highlight_colour, __gui_shadow_colour;
extern int __gui_text_colour;
extern void *__gui_font;
extern void *__gui_dest;

extern GUI_WM *__gui_wm;
extern GUI_WM __gui_default_wm;

extern JSContext *__gui_cx;


#endif /* __included_jsginter_h */
