/* import.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <jsapi.h>
#include "native.h"
#include "gui.h"
#include "guiinter.h"



/* as it turns out, `import' is a reserved keyword, so 
 * in the .js files this function is actually called `load'
 */
static NATIVE_FUNCTION(import)
{
    int i;
    JSString *str;
    const char *filename;
    JSScript *script;
    JSBool ok;
    jsval result;

    for (i=0; i<argc; i++) {
	str = JS_ValueToString(cx, argv[i]);
	if (!str)
	    return JS_FALSE;
	filename = JS_GetStringBytes(str);

	script = JS_CompileFile(cx, obj, filename);
	if (!script)
            ok = JS_FALSE;
        else {
            ok = JS_ExecuteScript(cx, obj, script, &result);
	    JS_DestroyScript(cx, script);
        }

	if (!ok)
	    return JS_FALSE;
    }

    return JS_TRUE;
}



NATIVE_FUNCTION_TABLE(import) = {
    /* Name     		Function		Number of arguments */
    { "load",			import, 		1 },
    { NULL }
};
