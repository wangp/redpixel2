/* jshelper.h */
#ifndef __included_jshelper_h
#define __included_jshelper_h

JSBool JS_GetSubProperty(JSContext *cx, JSObject *obj,
			 const char *name, jsval *vp);



/* inlined for extra speed */

extern inline 
int JS_GetPropertyInt(JSContext *cx, JSObject *obj, const char *name) 
{
    jsval val;
    if (!JS_GetProperty(cx, obj, name, &val))
	return 0;
    return JSVAL_TO_INT(val);
}
    


extern inline 
JSObject *JS_GetPropertyObject(JSContext *cx, JSObject *obj, const char *name)
{
    jsval val;
    if (!JS_GetProperty(cx, obj, name, &val))
	return 0;
    return JSVAL_TO_OBJECT(val);
}


#endif /* __included_jshelper_h */
