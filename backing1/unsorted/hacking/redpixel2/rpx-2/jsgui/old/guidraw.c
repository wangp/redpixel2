/* guidraw.c - widget drawing functions (Allegro) */


#include <allegro.h>
#include <jsapi.h>
#include "gui.h"
#include "guiinter.h"



static JSBool draw_basic(JSContext *cx, JSObject *obj, 
			 uintN argc, jsval *argv, jsval *rval)
{
    BITMAP *bmp;
    int x, y, w, h;
    jsval vp1, vp2;

    /* get bmp, x, y from args */
    if (!JS_ConvertArguments(cx, 2, argv, "uu", &x, &y))
	return JS_FALSE;

    /* get w, h from obj properties */
    if (!JS_GetProperty(cx, obj, "w", &vp1) ||
	!JS_GetProperty(cx, obj, "h", &vp2))
	return JS_FALSE;

    w = JSVAL_TO_INT(vp1);
    h = JSVAL_TO_INT(vp2);

    rectfill(__gui_dest, x, y, x+w-1, y+h-1, __gui_base_colour);
    return JS_TRUE;
}



static JSFunctionSpec gui_internal_members[] = {
    /* Name     	Function	Number of arguments */
    { "draw_basic",	draw_basic,	3 }
};



int __gui_draw_init_hook(JSContext *cx, JSObject *internal)
{
    return JS_DefineFunctions(cx, internal, gui_internal_members);
}
