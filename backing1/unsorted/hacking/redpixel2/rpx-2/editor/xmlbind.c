/* xmlbind.c - JS bindings for `expat' package
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdio.h>
#include <allegro.h>
#include <jsapi.h>
#include <xmlparse.h>
#include "native.h"
#include "gui.h"
#include "guiinter.h"



typedef struct userdata {
    JSContext *cx;
    JSObject *object;
    
    /* functions */
    jsval start_elem;
    jsval end_elem;
    jsval chdata;
} USERDATA; /* lazy */



/* used by the XML_set_blah_foo_foo before XML_parse */
static JSObject *_start_elem, *_end_elem;
static JSObject *_chdata, *_object;



static void start_element(void *userdata, const char *name, const char **_atts)
{
    USERDATA *udata = userdata;
    char **atts = (char **)_atts;
    JSContext *cx = udata->cx;
    JSString *str;
    JSObject *array;
    jsval argv[2], rval;
    
    if (JSVAL_IS_VOID(udata->end_elem) || JSVAL_IS_NULL(udata->end_elem))
	return;	
    
    /* create array filled with key:value pairs */
    array = JS_NewArrayObject(cx, 0, NULL);
    while (*atts) {
	char *key, *value;
	JSString *vstr;
	
	key = *atts++;
	value = *atts;
	vstr = JS_NewStringCopyZ(cx, value);
	JS_DefineProperty(cx, array, key, STRING_TO_JSVAL(vstr), 
			  NULL, NULL, JSPROP_ENUMERATE | JSPROP_EXPORTED);
	
	atts++;
    }
        
    str = JS_NewStringCopyZ(cx, name);
    argv[0] = STRING_TO_JSVAL(str);
    argv[1] = OBJECT_TO_JSVAL(array);
    JS_CallFunctionValue(cx, udata->object, udata->start_elem, 2, argv, &rval);
}


static void end_element(void *userdata, const char *name)
{
    USERDATA *udata = userdata;
    JSString *str;
    jsval argv[1], rval;
    
    if (JSVAL_IS_VOID(udata->end_elem) || JSVAL_IS_NULL(udata->end_elem))
	return;	
    
    str = JS_NewStringCopyZ(udata->cx, name);
    argv[0] = STRING_TO_JSVAL(str);
    JS_CallFunctionValue(udata->cx, udata->object, udata->end_elem, 1, argv, &rval);
}


static void character_data(void *userdata, const XML_Char *s, int len)
{
    USERDATA *udata = userdata;
    char *tmp;
    JSString *str;
    jsval argv[1], rval;
    
    if (JSVAL_IS_VOID(udata->chdata) || JSVAL_IS_NULL(udata->chdata))
	return;	
    
    tmp = malloc(len+1);
    memcpy(tmp, s, len);
    tmp[len+1] = 0;
    str = JS_NewStringCopyZ(udata->cx, tmp);
    free(tmp);
    
    argv[0] = STRING_TO_JSVAL(str);
    JS_CallFunctionValue(udata->cx, udata->object, udata->chdata, 1, argv, &rval);
}



static int _xml_parse(JSContext *cx, char *file)
{
    FILE *fp;
    XML_Parser parser;
    USERDATA userdata;
    char buf[BUFSIZ];
    size_t len;
    int done, ret = 1;
    
    fp = fopen(file, "r");
    if (!fp)
	return 0;
        
    parser = XML_ParserCreate(NULL);
    
    userdata.cx = cx;
    userdata.object = _object;
    userdata.start_elem = OBJECT_TO_JSVAL(_start_elem);
    userdata.end_elem = OBJECT_TO_JSVAL(_end_elem);
    userdata.chdata = OBJECT_TO_JSVAL(_chdata);
    XML_SetUserData(parser, &userdata);
    
    XML_SetElementHandler(parser, start_element, end_element);
    XML_SetCharacterDataHandler(parser, character_data);

    do {
	len = fread(buf, 1, sizeof(buf), fp);
	done = len < sizeof(buf);
	if (!XML_Parse(parser, buf, len, done)) {

#if 0
	    fprintf(stderr, "%s at line %d\n",
		    XML_ErrorString(XML_GetErrorCode(parser)),
		    XML_GetCurrentLineNumber(parser));
#else
	    do {
		char err[256];
		sprintf(err, "%s in %s:%d:%d",
			XML_ErrorString(XML_GetErrorCode(parser)),
			file,
			XML_GetCurrentLineNumber(parser),
			XML_GetCurrentColumnNumber(parser));
		alert(err, "", "parsing aborted", "Ok", NULL, 13, 27);
	    } while (0);
#endif	    
	    
	    done = 1;
	    ret = 0;
	}
    } while (!done);
    
    XML_ParserFree(parser);
    fclose(fp);
   
    return ret;
}



static NATIVE_FUNCTION(XML_set_element_handler)
{
    _start_elem = JSVAL_TO_OBJECT(argv[0]);
    _end_elem = JSVAL_TO_OBJECT(argv[1]);
    _object = obj;
    return JS_TRUE;
}

static NATIVE_FUNCTION(XML_set_character_data_handler)
{
    _chdata = JSVAL_TO_OBJECT(argv[0]);
    _object = obj;
    return JS_TRUE;
}



static NATIVE_FUNCTION(XML_parse)
{
    JSString *str;
    char *filename;
    
    str = JSVAL_TO_STRING(argv[0]);
    filename = JS_GetStringBytes(str);
    if (!filename)
	return JS_FALSE;

    return _xml_parse(cx, filename) ? JS_TRUE : JS_FALSE;
}



NATIVE_FUNCTION_TABLE(xmlbind) = {
    /* Name     			Function			Number of arguments */
    { "set_element_handler",		XML_set_element_handler,	2 },
    { "set_character_data_handler",	XML_set_character_data_handler,	1 },
    { "parse", 				XML_parse,			1 },
    { NULL }
};

