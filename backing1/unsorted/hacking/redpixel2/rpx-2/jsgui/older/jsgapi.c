/* jsgapi.c - API functions */


#include <stdlib.h>
#include <js/jsapi.h>
#include "jsgui.h"
#include "jsginter.h"



JSBool __gui_jsgwdraw_gui_root_hook(JSContext *cx, JSObject *guiroot, JSObject *internal);



GUI_WM *__gui_wm = &__gui_default_wm;

int __gui_width, __gui_height, __gui_bpp;
int __gui_base_colour, __gui_highlight_colour, __gui_shadow_colour;
int __gui_text_colour; 
void *__gui_font;
void *__gui_dest = NULL;

JSContext *__gui_cx;



static JSClass gui_root_class = {
    "GUIRoot", 0,
    JS_PropertyStub, JS_PropertyStub,   /* addProperty, delProperty */
    JS_PropertyStub, JS_PropertyStub,   /* getProperty, setProperty */
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub, JS_FinalizeStub
};



static JSClass internal_class = {
    "__internal", 0,
    JS_PropertyStub, JS_PropertyStub,   /* addProperty, delProperty */
    JS_PropertyStub, JS_PropertyStub,   /* getProperty, setProperty */
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub, JS_FinalizeStub
};



static char gui_script_path[1024] = "jsgui.js";
static JSScript *gui_script;
static JSObject *gui_root_object;

static int virgin = 1;



/*
 * exported functions
 */

static JSBool guiroot_new_client(JSContext *cx, JSObject *obj, 
				 uintN argc, jsval *argv, jsval *rval)
{
    int w, h;
    
    if (!JS_ConvertArguments(cx, 2, argv, "uu", &w, &h))
      return JS_FALSE;
    
    *rval = INT_TO_JSVAL(__gui_wm->new_client(w, h));
    return JS_TRUE;
}

static JSBool guiroot_remove_client(JSContext *cx, JSObject *obj, 
				    uintN argc, jsval *argv, jsval *rval)
{
    int id;
    if (!JSVAL_IS_INT(argv[0]))
      return JS_FALSE;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->remove_client(id);
    return JS_TRUE;
}

static JSBool guiroot_mark_client(JSContext *cx, JSObject *obj, 
				  uintN argc, jsval *argv, jsval *rval)
{
    int id;
    JS_ValueToInt32(cx, argv[0], &id);
    __gui_wm->mark_client(id);
    return JS_TRUE;
}

/* client/wm exported to GUIRoot.__internal */
static JSFunctionSpec guiroot_internal_wm_members[] = {
    /* Name     	Function		Number of arguments */
    { "new_client",	guiroot_new_client,		3 },
    { "remove_client", 	guiroot_remove_client,		1 },
    { "mark_client", 	guiroot_mark_client,		1 }
};



/* set jsgui.js location.  Future versions may have 
 * better ways of doing this.  (e.g. builtin to C code)
 */
void gui_set_jsgui_location(char *path)
{
    strcpy(gui_script_path, path);
}



/* set window manager (before call to gui_init) */
void gui_set_window_manager(GUI_WM *newwm)
{
    if (virgin)
      __gui_wm = newwm;
}



/* init a few things, ready system */
int gui_init(JSContext *cx, JSObject *glob,
	     int display_w, int display_h, int colour_depth)
{
    JSObject *internal;
    jsval result;
    
    if (!virgin)
      return 0;

    __gui_width = display_w;
    __gui_height = display_h;
    __gui_bpp = colour_depth;

    /* add GUIRoot object */
    gui_root_object = JS_DefineObject(cx, glob, "GUIRoot",
				      &gui_root_class, NULL, 0);
    
    /* add sub-property '__internal' */
    internal = JS_DefineObject(cx, gui_root_object, "__internal",
			       &internal_class, NULL, 0);
    if (!internal)
      return 0;

    /* define some properties */
    JS_DefineProperty(cx, gui_root_object, "x", INT_TO_JSVAL(0), JS_PropertyStub, JS_PropertyStub, 0);
    JS_DefineProperty(cx, gui_root_object, "y", INT_TO_JSVAL(0), JS_PropertyStub, JS_PropertyStub, 0);
    JS_DefineProperty(cx, gui_root_object, "w", INT_TO_JSVAL(__gui_width), JS_PropertyStub, JS_PropertyStub, 0);
    JS_DefineProperty(cx, gui_root_object, "h", INT_TO_JSVAL(__gui_height), JS_PropertyStub, JS_PropertyStub, 0);
    JS_DefineProperty(cx, internal, "bpp", INT_TO_JSVAL(__gui_bpp), JS_PropertyStub, JS_PropertyStub, 0);

    /* define native functions (wm) */
    if (!JS_DefineFunctions(cx, internal, guiroot_internal_wm_members))
      return 0;

    /* more stuff that we need to add to __internal */
    if (!__gui_jsgwdraw_gui_root_hook(cx, gui_root_object, internal))
      return 0;
    
    /* load the script first, so if it fails, it fails before wm->init */
    gui_script = JS_CompileFile(cx, glob, gui_script_path);
    if (!gui_script)
      return 0;

    /* init our WM */
    if (!__gui_wm->init(display_w, display_h, colour_depth)) {
	JS_DestroyScript(cx, gui_script);
	return 0;
    }

    /* run the jsgui.js script to finish things off */
    __gui_cx = cx;    
    JS_ExecuteScript(cx, glob, gui_script, &result);

    atexit(gui_exit);
    return 1;
}



/* shutdown the system (this is atexit'd by gui_init) */
void gui_exit()
{
    if (!virgin) {
	__gui_wm->shutdown();
	JS_DestroyScript(__gui_cx, gui_script);
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


