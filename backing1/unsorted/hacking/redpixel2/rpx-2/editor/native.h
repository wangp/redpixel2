/* native.h */
#ifndef __included_native_h
#define __included_native_h

void export_native_functions(JSContext *cx, JSObject *glob);


/*
 * makes the code look a bit cleaner
 */

#define NATIVE_FUNCTION(name) 		JSBool name(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
#define NATIVE_FUNCTION_TABLE(x)	JSFunctionSpec __native__##x[]


#endif /* __included_native_h */
