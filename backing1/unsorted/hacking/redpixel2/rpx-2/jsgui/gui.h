#ifndef __included_gui_h
#define __included_gui_h


#include <jsapi.h>
#include <allegro.h>


int gui_init(JSContext *cx, JSObject *glob, char *module_path, int w, int h, int bpp);
void gui_exit();
void gui_set_colours(int base, int hl, int sh);
void gui_set_text_colour(int textcol);
void gui_set_default_font(FONT *f);
void gui_set_display(BITMAP *bmp);

int gui_update();


#endif /* __included_gui_h */
