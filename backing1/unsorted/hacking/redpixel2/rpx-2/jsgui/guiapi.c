/* guiapi.c 
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <jsapi.h>
#include <allegro.h>
#include "gui.h"
#include "guiinter.h"



JSScript  *__gui_script;
JSContext *__gui_cx;



int __gui_width, __gui_height, __gui_bpp;
int __gui_base_colour, __gui_highlight_colour, __gui_shadow_colour;
int __gui_text_colour; 
FONT *__gui_font;

BITMAP *__gui_dest = NULL;

struct gui_wm *__gui_wm = &__gui_default_wm;



static int virgin = 1;



/*
 * Some glue functions to interface with our WM
 */
 
static JSBool gui_native_new_client(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSObject *jobj;
    int w, h;
    
    if (!JS_ConvertArguments(cx, 3, argv, "ouu", &jobj, &w, &h))
	return JS_FALSE;

    *rval = INT_TO_JSVAL(__gui_wm->new_client(jobj, w, h));	/* return id */
    return JS_TRUE;
}



static JSBool gui_native_remove_client(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int id;
    if (!JSVAL_IS_INT(argv[0]))
      return JS_FALSE;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->remove_client(id);
    return JS_TRUE;
}



static JSBool gui_native_move_client(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int id, rel_x, rel_y;
    
    if (!JS_ConvertArguments(cx, 3, argv, "uuu", &id, &rel_x, &rel_y))
	return JS_FALSE;

    __gui_wm->move_client(id, rel_x, rel_y);
    return JS_TRUE;
}



static JSBool gui_native_mark_client(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int id;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->mark_client(id);
    return JS_TRUE;
}



static JSBool gui_native_raise_client(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int id;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->mark_client(id);
    return JS_TRUE;
}



static JSBool gui_native_lower_client(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int id;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->mark_client(id);
    return JS_TRUE;
}



static JSBool gui_client_special(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int cmd, id;
    
    if (!JS_ConvertArguments(cx, 2, argv, "uu", &id, &cmd))
	return JS_FALSE;
    
    __gui_wm->client_special(id, cmd);
    return JS_TRUE;
}



static JSFunctionSpec gui_native_members[] = {
    /* Name     	Function			Number of arguments */
    { "new_client",	gui_native_new_client,		3 },
    { "remove_client", 	gui_native_remove_client,	1 },
    { "move_client", 	gui_native_move_client,		3 },
    { "mark_client", 	gui_native_mark_client,		1 },
    { "raise_client", 	gui_native_raise_client,	1 },
    { "lower_client", 	gui_native_lower_client,	1 },
    { "client_special", gui_client_special,		2 },
    { NULL }
};



/* gui_init:
 *  Init a few things, ready system
 */
int gui_init(JSContext *cx, JSObject *glob, char *module_path, int w, int h, int bpp)
{
    JSObject *native;
    jsval val;
    
    if (!virgin)
      return 0;
    
    __gui_width = w;
    __gui_height = h;
    __gui_bpp = bpp;
    __gui_cx = cx;    

    
    /* load our gui.js module and execute */
    __gui_script = JS_CompileFile(cx, glob, module_path);
    if (!__gui_script)
	return 0;
    JS_ExecuteScript(cx, glob, __gui_script, &val);
    
    
    /* set up w, h */
    if (JS_GetSubProperty(cx, glob, "GUI.__init_wh", &val)) {
	jsval argv[2], rval;
	
    	argv[0] = INT_TO_JSVAL(w);
    	argv[1] = INT_TO_JSVAL(h);
	JS_CallFunctionValue(cx, glob, val, 2, argv, &rval);
    }
    
    
    /* grab pointer to GUI.native */
    if (JS_GetSubProperty(cx, glob, "GUI.__native", &val))
	native = JSVAL_TO_OBJECT(val);
    else 
	return 0;
        
    /* use it to define native functions */
    if (!JS_DefineFunctions(cx, native, gui_native_members))
      return 0;
    __gui_draw_init_hook(cx, native);
    __gui_wm->init(w, h, bpp);
    
    
    /* finally, start our timer */
    __gui_init_ticker();

    
    virgin = 0;
    atexit(gui_exit);
    return 1;
}



/* gui_exit:
 *  Shutdown the system (this is atexit'd by gui_init) 
 */
void gui_exit()
{
    if (!virgin) {
	__gui_wm->shutdown();
	
	JS_DestroyScript(__gui_cx, __gui_script);
	virgin = 1;
    }
}



/* 
 * various colour/font/display setting routines 
 */

void gui_set_colours(int base, int hl, int sh)
{
    if (base != -1) __gui_base_colour = base;
    if (hl != -1) __gui_highlight_colour = hl;
    if (sh != -1) __gui_shadow_colour = sh;
}


void gui_set_text_colour(int textcol)
{
    __gui_text_colour = textcol;
}


void gui_set_default_font(FONT *f)
{
    __gui_font = f;
}


void gui_set_display(BITMAP *p)
{
    __gui_dest = p;
}



