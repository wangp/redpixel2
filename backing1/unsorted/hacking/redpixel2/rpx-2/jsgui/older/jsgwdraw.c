/* jsgwdraw.c - (Allegro-compliant) widget drawing functions */


#include <js/jsapi.h>
#include <allegro.h>
#include "jsgui.h"
#include "jsginter.h"


/*
 * Widget drawing helpers
 */
 
static void draw_base(BITMAP *dest, int x1, int y1, int x2, int y2)
{
    rectfill(dest, x1, y1, x2, y2, __gui_base_colour);
}


static void draw_up_borders(BITMAP *dest, int x1, int y1, int x2, int y2)
{
    hline(dest, x1, y1, x2, __gui_highlight_colour);
    vline(dest, x1, y1, y2, __gui_highlight_colour);
    hline(dest, x1, y2, x2, __gui_shadow_colour);
    vline(dest, x2, y1, y2, __gui_shadow_colour);
}


static void draw_down_borders(BITMAP *dest, int x1, int y1, int x2, int y2)
{
    rectfill(dest, x1, y1, x2, y2, __gui_base_colour);
    hline(dest, x1, y1, x2, __gui_shadow_colour);
    vline(dest, x1, y1, y2, __gui_shadow_colour);
    hline(dest, x1, y2, x2, __gui_highlight_colour);
    vline(dest, x2, y1, y2, __gui_highlight_colour);
}


static void draw_text_aligned(BITMAP *dest, int x1, int y1, int x2, int y2, 
			      char *s, int align_x, int align_y)
{
    int w, h, x, y;
    w = text_length(__gui_font, s);
    h = text_height(__gui_font);
    
    switch (align_x) {
      case GUI_ALIGN_LEFT:	x = x1; break;
      case GUI_ALIGN_RIGHT:	x = x2 - w; break;
      case GUI_ALIGN_CENTRE:    x = (x1 + x2) / 2 + w / 2; break;
    }
    
    switch (align_y) {
      case GUI_ALIGN_TOP:	y = y1; break;
      case GUI_ALIGN_BOTTOM:	y = y2 - h; break;
      case GUI_ALIGN_CENTRE:    y = (y1 + y2) / 2 + h / 2; break;
    }
    
    textout(dest, __gui_font, s, x, y, __gui_text_colour);
}



/*
 * Widget drawers
 */
 
static JSBool __gui_draw_pushbutton(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv, jsval *rval)
{
    BITMAP *bmp;
    int x1, y1, x2, y2, w, h, dp;
    jsval jw, jh, jdp, jlbl;
    JSString *str;
    char *lbl;
    
    JS_ValueToInt32(cx, argv[0], (int *)&bmp);
    JS_ValueToInt32(cx, argv[1], &x1);
    JS_ValueToInt32(cx, argv[2], &y1);
    
    JS_GetProperty(cx, obj, "w", &jw);
    JS_GetProperty(cx, obj, "h", &jh);
    JS_GetProperty(cx, obj, "depressed", &jdp);
    
    JS_ValueToInt32(cx, jw, &w);
    JS_ValueToInt32(cx, jh, &h);
    JS_ValueToInt32(cx, jdp, &dp);
    
    JS_GetProperty(cx, obj, "label", &jlbl);
    str = JS_ValueToString(cx, jlbl);
    lbl = JS_GetStringBytes(str);
    
    x2 = x1 + w - 1;
    y2 = y1 + h - 1;
    
    draw_base(bmp, x1, y1, x2, y2);
    
    if (!dp)
      draw_up_borders(bmp, x1, y1, x2, y2);
    else 
      draw_down_borders(bmp, x1, y1, x2, y2);

    return JS_TRUE;
}


/* members that will be made available to our (internal) scripts */
static JSFunctionSpec guiroot_internal_members[] = {
    /* Name     		Function		Number of arguments */
    { "DrawPushButton",		__gui_draw_pushbutton, 	1 }
};


JSBool __gui_jsgwdraw_gui_root_hook(JSContext *cx, JSObject *guiroot, JSObject *internal)
{
    if (!JS_DefineFunctions(cx, internal, guiroot_internal_members))
      return JS_FALSE;
    
    return JS_TRUE;
}
