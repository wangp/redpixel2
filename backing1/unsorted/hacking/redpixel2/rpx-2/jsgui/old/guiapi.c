/* guiapi.c - API functions */


#include <stdlib.h>
#include <jsapi.h>
#include "gui.h"
#include "guiinter.h"
#include "jshelper.h"
#include "guidraw.h"



GUI_WM *__gui_wm = &__gui_default_wm;

int __gui_width, __gui_height, __gui_bpp;
int __gui_base_colour, __gui_highlight_colour, __gui_shadow_colour;
int __gui_text_colour; 
void *__gui_font;
void *__gui_dest = NULL;

JSScript  *__gui_script;
JSContext *__gui_cx;



static int virgin = 1;



static JSBool gui_internal_new_client(JSContext *cx, JSObject *obj, 
				      uintN argc, jsval *argv, jsval *rval)
{
    JSObject *jobj;
    int w, h;
    
    if (!JS_ConvertArguments(cx, 3, argv, "ouu", &jobj, &w, &h))
	return JS_FALSE;

    *rval = INT_TO_JSVAL(__gui_wm->new_client(jobj, w, h)); /* return id */
    return JS_TRUE;
}



static JSBool gui_internal_remove_client(JSContext *cx, JSObject *obj, 
					 uintN argc, jsval *argv, jsval *rval)
{
    int id;
    if (!JSVAL_IS_INT(argv[0]))
      return JS_FALSE;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->remove_client(id);
    return JS_TRUE;
}



static JSBool gui_internal_mark_client(JSContext *cx, JSObject *obj, 
				       uintN argc, jsval *argv, jsval *rval)
{
    int id;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->mark_client(id);
    return JS_TRUE;
}



static JSFunctionSpec gui_internal_members[] = {
    /* Name     	Function			Number of arguments */
    { "new_client",	gui_internal_new_client,	3 },
    { "remove_client", 	gui_internal_remove_client,	1 },
    { "mark_client", 	gui_internal_mark_client,	1 }
};



/* set window manager (before call to gui_init) */
void gui_set_window_manager(GUI_WM *newwm)
{
    if (virgin)
	__gui_wm = newwm;
}



/* init a few things, ready system */
int gui_init(JSContext *cx, JSObject *glob, char *module_path,
	     int w, int h, int bpp)
{
    JSObject *internal;
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
    
    /* grab pointer to GUI.internal */
    if (JS_GetSubProperty(cx, glob, "GUI.internal", &val))
      internal = JSVAL_TO_OBJECT(val);
    else 
      return 0;
        
    /* define native functions (wm) */
    if (!JS_DefineFunctions(cx, internal, gui_internal_members))
      return 0;

    if (!__gui_draw_init_hook(cx, internal))
      return 0;

    /* start up our preferred window manager */
    __gui_wm->init(w, h, bpp);

    atexit(gui_exit);
    return 1;
}



/* shutdown the system (this is atexit'd by gui_init) */
void gui_exit()
{
    if (!virgin) {
	__gui_wm->shutdown();
	JS_DestroyScript(__gui_cx, __gui_script);
	virgin = 1;
    }
}



/* various colour/font/display setting routines */

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


void gui_set_font(void *defaultfont)
{
    __gui_font = defaultfont;
}


void gui_set_display(void *p)
{
    __gui_dest = p;
}



/* 
 * debugging
 */

int __gui_spew(JSObject *obj)	/* say cheese */
{
    jsval vp, rval;
    JSString *str;

    JS_GetProperty(__gui_cx, obj, "this.type", &vp);
    str = JS_ValueToString(__gui_cx, vp);
    if (str)
      printf("::: spew ::::::::::::%s:::::::::::::::::\n", JS_GetStringBytes(str));
    
    JS_CallFunctionName(__gui_cx, obj, "this.spew", 0, NULL, &rval);
    /*
    if (JS_GetSubProperty(__gui_cx, obj, "spew", &vp)) {
	shout("spew!");
	JS_CallFunctionValue(__gui_cx, obj, vp, 0, NULL, &rval);
	return 1;
	}*/
    
    return 0;
}
