/* basehelp.c - native Base helper functions
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdio.h>
#include <string.h>
#include <jsapi.h>
#include "gui.h"
#include "guiinter.h"
#include "wumpus.h"
#include "native.h"



static NATIVE_FUNCTION(make_base_array)
{
    JSObject *array;
    jsint index=0;
    BUCKET *dat;
    char *tiles = "/tiles/";
    int len, i;
    JSString *str;
    
    array = JSVAL_TO_OBJECT(argv[0]);
    
    len = strlen(tiles);
    
    for (i=0; i<fat_gellopy.size; i++) {
	dat = fat_gellopy.table[i];
	
	while (dat) {
	    if ((strlen(dat->key) > len) && (strncmp(dat->key, tiles, len) == 0)) {
		//printf("%s\n", dat->key);

		str = JS_NewStringCopyZ(cx, dat->key);
		
		JS_DefineElement(cx, array, index++,
				 STRING_TO_JSVAL(str),
				 NULL, NULL, JSPROP_ENUMERATE | JSPROP_EXPORTED);
	    }
	    
	    dat = dat->next;
	}
    }
    
    return JS_TRUE;
}



NATIVE_FUNCTION_TABLE(basehelp) = {
    /* Name     		Function		Number of arguments */
    { "make_base_array",	make_base_array, 	1 },
    { NULL }
};
