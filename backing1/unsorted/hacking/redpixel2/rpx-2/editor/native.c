/* native.c - export native functions into GUI.__native
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <jsapi.h>
#include "jshelper.h"
#include "native.h"



#define E(x)	__native__##x



extern NATIVE_FUNCTION_TABLE(basehelp);
extern NATIVE_FUNCTION_TABLE(draw);
extern NATIVE_FUNCTION_TABLE(import);
extern NATIVE_FUNCTION_TABLE(xmlbind);



static JSFunctionSpec *gui_native_table[] = {   /* goes to GUI.__native */
    E(basehelp),
    E(draw),
    NULL
};


    
static JSFunctionSpec *xml_table[] = {		/* goes to `XML' */
    E(xmlbind),
    NULL
};



static JSFunctionSpec *glob_table[] = {		/* goes to global namespace */
    E(import),
    NULL
};



static JSClass dummy_class = {
    "dummy", 0,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub
};


static JSObject *_create_object(JSContext *cx, JSObject *obj, char *name)
{
    return JS_DefineObject(cx, obj, name, &dummy_class, NULL, 0);
}


static void _export(JSContext *cx, JSObject *obj, JSFunctionSpec **table)
{
    int i;
    for (i=0; table[i]; i++)
	JS_DefineFunctions(cx, obj, table[i]);
}


    
void export_native_functions(JSContext *cx, JSObject *glob)
{
    JSObject *obj;
    jsval val;
    
    /* GUI.__native */
    if (JS_GetSubProperty(cx, glob, "GUI.__native", &val)) {
	obj = JSVAL_TO_OBJECT(val);
	_export(cx, obj, gui_native_table);
    }
    
    /* XML.foo */
    obj = _create_object(cx, glob, "XML");
    _export(cx, obj, xml_table);
    
    /* global ``namespace'' */
    _export(cx, glob, glob_table);
}

