/* stub.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <jsapi.h>
#include <allegro.h>
#include "gui.h"
#include "native.h"
#include "colours.h"
#include "datalist.h"



/* FIXME: these paths should not be so... */
#define GUI_PATH 	"jsgui/gui.js"
#define RPXEDIT_PATH	"scripts/rpxedit.js"



/* This is a function that we will use in our scripts */
static JSBool native_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
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



/* This is a function that we will use in our scripts */
static JSBool native_alert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i;
    JSString *str;
    char lines[3][512] = { "", "", "" };

    for (i = 0; (i < argc) && (i < 3); i++) {
	str = JS_ValueToString(cx, argv[i]);
	if (!str)
	    return JS_FALSE;
	else
	    sprintf(lines[i], "%s", JS_GetStringBytes(str));
    }
    
    alert(lines[0], lines[1], lines[2], "Ok", NULL, 13, 27);
    
    return JS_TRUE;
}



/* Native C functions that will be made available to our scripts */
static JSFunctionSpec native_functions[] = {
    /* Name     	Function	Number of arguments */
    { "print",		native_print,	0 },
    { "alert",		native_alert,	0 },
    { NULL }
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

    
    /*
     * Initialise JavaScript
     */
      
    /* initialise JavaScript runtime */
    rt = JS_NewRuntime(8 * 1024 * 1024);
    if (!rt) {
	fprintf(stderr, "Couldn't create a JavaScript runtime.\n");
	return 1;
    }
    
    /* create an execution context and associate it with the JS run time */
    cx = JS_NewContext(rt, 4 * 1024 * 1024);
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
    

    /*
     * Initialise Allegro
     */
      
    allegro_init();
    install_timer();
    install_keyboard();
    
    if (install_mouse() < 0) {
	allegro_message("Error initialising mouse\n%s\n", allegro_error);
	return 1;
    }

    set_color_depth(16);
   if (set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0) != 0) {
      if (set_gfx_mode(GFX_AUTODETECT, 1024, 768, 0, 0) != 0) {
	    allegro_message("Error setting video mode\n%s\n", allegro_error);
	    return 1;
	}
    }

    gui_set_display(screen);
    show_mouse(screen);

    
    /*
     * Initialise pretty colours
     */
      
    /* set a common colour scheme for Allegro dialogs and JSGUI */
    init_thepaw_colours();
    
    gui_fg_color = paw_blue;
    gui_bg_color = paw_black;
    
    gui_set_colours(paw_black, paw_lightgrey, paw_darkgrey);
    gui_set_text_colour(paw_blue);
    
    gui_set_default_font(font);
    text_mode(-1);
    

    /*
     * Initialise JSGUI
     */
      
    /* initialise the JSGUI system */
    replace_filename(path, argv[0], GUI_PATH, 1024);
    if (!gui_init(cx, glob, path, SCREEN_W, SCREEN_H, bitmap_color_depth(screen))) {
	allegro_message("Error initialising jsgui\n");
	return 1;
    }
    
    /* stick some functions into GUI.__native
     * (see native.c)
     */
    export_native_functions(cx, glob);
    

    /*
     * Initialise the data list system
     *  (actually Wumpus and fat gellopy in disguise)
     */
    
    rpx_data_init();

    do {
	void _load(char *filename, int attrib, int param) {
	    rpx_load_data(filename, "/tiles/");
	}
	for_each_file("data/tiles/*.dat", FA_RDONLY | FA_ARCH, _load, 0);
    } while (0);

    
    /*
     * Finally the main loop
     */
      
    /* compile and initialise our example */
    script = JS_CompileFile(cx, glob, RPXEDIT_PATH);
    if (script) {
	JS_ExecuteScript(cx, glob, script, &result);
	
	/* do the GUI loop */
	while (gui_update())
	    ;
	
	JS_DestroyScript(cx, script);
    }
  
    
    /*
     * Bye bye.
     */
    
    rpx_data_cleanup();
      
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
    
    return 0;
}

END_OF_MAIN();
