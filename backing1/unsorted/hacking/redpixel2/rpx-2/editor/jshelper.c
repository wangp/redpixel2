/*
 * JS Engine helper (convenience) routines.
 */


#include <string.h>
#include <stdlib.h>
#include <jsapi.h>


/*
 *  Same as JS_GetProperty, except follows this.that.property style paths.
 */
JSBool JS_GetSubProperty(JSContext *cx, JSObject *obj, 
			 const char *name, jsval *vp)
{
    JSObject *cur;
    JSBool ret = JS_FALSE;
    char *buf, *p, *dot = ".";
    
    buf = strdup(name);
    if (!buf)
      return JS_FALSE;
    
    p = strtok(buf, dot);
    cur = obj;

    while (p) {
	ret = JS_GetProperty(cx, cur, p, vp);
	if (!ret)
	  goto quit;
	p = strtok(NULL, dot);
	cur = JSVAL_TO_OBJECT(*vp);
    } 

 quit:			   
    free(buf);
    return ret;
}
