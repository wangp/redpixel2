/* JS GUI example program, derived from ex1.c */


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <jsapi.h>
#include <allegro.h>
#include "gui.h"
#include "colours.h"



/* This is a function that we will use in our scripts */
static JSBool _print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
   uintN i;
   JSString *str;

   for (i = 0; i < argc; i++) {
      str = JS_ValueToString(cx, argv[i]);
      if (!str)
	 return JS_FALSE;
      else
	 printf("%s%s", i ? " " : "", JS_GetStringBytes(str));
   }

   return JS_TRUE;
}



/* Wait a number of msecs */
static JSBool _delay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
   int msec;
   clock_t start, now;
   
   if (JS_ValueToInt32(cx, argv[0], &msec) != JS_TRUE)
      return JS_FALSE;
   
   start = clock();
   for (;;) {
      now = clock();
      if (((float)(now - start) / CLOCKS_PER_SEC) * 1000 >= msec)
	 return JS_TRUE;
   } 
}


   
/* Get character input from stdin */
static JSBool _getchar(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{   
   *rval = INT_TO_JSVAL(getchar());
   return JS_TRUE;
}



/* Native C functions that will be made available to our scripts */
static JSFunctionSpec native_functions[] = {
    /* Name     	Function	Number of arguments */
    { "print",		_print,		0 },
    { "delay", 		_delay,		1 },
    { "getchar", 	_getchar, 	1 }
};



/* Global class.  These methods and properties 
 * are inherited by all other objects.
 */
static JSClass global_class = {
    "global", 0,
    JS_PropertyStub, JS_PropertyStub,   /* addProperty, delProperty */
    JS_PropertyStub, JS_PropertyStub,   /* getProperty, setProperty */
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub, JS_FinalizeStub
};



/* Our error callback */
static void report_error(JSContext *cx, const char *msg, JSErrorReport *report)
{
    if (JSREPORT_IS_WARNING(report->flags)) 
	fprintf(stderr, "Warning:");
    else
	fprintf(stderr, "Error:");
   
    if (report->filename) {
	fprintf(stderr, " %s", report->filename);
	if (!report->lineno) fprintf(stderr, ":");
    }

    if (report->lineno)
	fprintf(stderr, " (%u):", report->lineno);
    
    fprintf(stderr, " %s\n", msg);
}



static char *strcatf(char *s1, char *fmt, ...)
{
    char *p = s1;
    va_list va;
    
    while (*p)
	p++;
    va_start(va, fmt);
    vsprintf(p, fmt, va);
    va_end(va);
    return s1;
}



/* Our graphical error callback */
static void report_error_alert(JSContext *cx, const char *msg, JSErrorReport *report)
{
    char buf[1024];
    
    if (JSREPORT_IS_WARNING(report->flags))
      strcpy(buf, "Warning:");
    else
      strcpy(buf, "Error:");
   
    if (report->filename) {
	strcatf(buf, " %s", report->filename);
	if (!report->lineno) strcat(buf, ":");
    }

    if (report->lineno)
      strcatf(buf, " (%u):", report->lineno);
    
    strcatf(buf, " %s", msg);
    
    alert(buf, "", "", "Argh!", NULL, 13, 27);
}



int main(int argc, char **argv) 
{
    char path[1024];
    JSRuntime *rt;
    JSContext *cx;
    JSObject *glob;
    JSScript *script;
    jsval result;
      
    /* initialise JavaScript runtime. */
    rt = JS_NewRuntime(8 * 1024 * 1024);
    if (!rt) {
	fprintf(stderr, "Couldn't create a JavaScript runtime.\n");
	return 1;
    }
    
    /* create an execution context and associate it with the JS run time */
    cx = JS_NewContext(rt, 1024 * 1024);
    if (!cx) {
	fprintf(stderr, "Couldn't create a JavaScript execution context.\n");
	return 1;
    }
   
    /* set a function for the engine to report errors with */
    JS_SetErrorReporter(cx, report_error_alert);

    /* create the global object */
    glob = JS_NewObject(cx, &global_class, NULL, NULL);
    if (!glob) {
	fprintf(stderr, "Couldn't create a global object.\n");
	return 1;
    }
    
    /* initialise the standard JavaScript classes */
    if (!JS_InitStandardClasses(cx, glob)) {
	fprintf(stderr, "Couldn't initialise standard classes.\n");
	return 1;
    }
   
    /* add native functions to global object */
    if (!JS_DefineFunctions(cx, glob, native_functions)) {
	fprintf(stderr, "Couldn't define native functions.\n");
	return 1;
    }
    
    /* initialise Allegro */
    allegro_init();
    install_mouse();
    install_timer();
    install_keyboard();
    
    set_color_depth(16);
    set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0);
    text_mode(-1);
    show_mouse(screen);
    
    init_thepaw_colours();
    gui_fg_color = paw_blue;
    gui_bg_color = paw_black;
    
    /* initialise the JSGUI system */
    replace_filename(path, argv[0], "gui.js", 1024);
    gui_init(cx, glob, path, SCREEN_W, SCREEN_H, bitmap_color_depth(screen));
    gui_set_display(screen);

    /* compile, execute, destroy */
    script = JS_CompileFile(cx, glob, "example.js");
    JS_ExecuteScript(cx, glob, script, &result);

    /* do the GUI loop */
    while (gui_update())
	;

    JS_DestroyScript(cx, script);
   
    /* bye bye */
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
    
    return 0;
}
