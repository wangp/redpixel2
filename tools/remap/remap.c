/* remap.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "lua.h"
#include "lualib.h"


/*----------------------------------------------------------------------*/
/*			lua_checkargs					*/
/*----------------------------------------------------------------------*/

/*
 * Function to validate arguments to an exported C function.
 * 
 * ARGTYPES contains the types of each of the arguments on top of the
 * stack to be validated against.  Each character can be one of:
 *
 *	u - the next argument should be userdata
 *  	N - nil
 *  	n - number
 * 	s - string
 *  	t - table
 *  	f - function
 *      - - none
 *  	* - anything
 *
 * In addition, [ and ] surrounding multiple characters can be used to
 * specify multiple valid types for the one argument.
 */

static inline int valid (lua_State *L, char argtype, int index)
{
    int type;

    switch (argtype) {
	case 'u': type = LUA_TUSERDATA; break;
	case 'N': type = LUA_TNIL; break;
	case 'n': type = LUA_TNUMBER; break;
	case 's': type = LUA_TSTRING; break;
	case 't': type = LUA_TTABLE; break;
	case 'f': type = LUA_TFUNCTION; break;
	case '-': type = LUA_TNONE; break;
	default: return 1;
    }

    return (lua_type (L, index) == type);
}

int lua_checkargs (lua_State *L, const char *argtypes)
{
    const char *a;
    int index;

    for (a = argtypes, index = 1; *a; a++, index++)
	if (*a == '[') {
	    a++;
	    while (*a != ']') {
		if (valid (L, *a, index)) break;
		a++;
	    }
	    if (*a == ']') return 0;
	    while (*a != ']') a++;
	}
	else if (!valid (L, *a, index)) 
	    return 0;

    return 1;
}


/*----------------------------------------------------------------------*/
/*			Binary shift bindings				*/
/*----------------------------------------------------------------------*/

static int bind_shl(lua_State *L)
{
	int value;
	int shift;
	int ret;
	if (!lua_checkargs(L, "nn")) goto error;
	value = lua_tonumber(L, 1);
	shift = lua_tonumber(L, 2);
	ret = value << shift;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_shr(lua_State *L)
{
	int value;
	int shift;
	int ret;
	if (!lua_checkargs(L, "nn")) goto error;
	value = lua_tonumber(L, 1);
	shift = lua_tonumber(L, 2);
	ret = value >> shift;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

#define DO_SHIFT_REGISTRATION(L)  \
	lua_register(L, "shl", bind_shl); \
	lua_register(L, "shr", bind_shr);


/*----------------------------------------------------------------------*/
/*			Packfile bindings				*/
/*----------------------------------------------------------------------*/

static int bind_pack_fopen(lua_State *L)
{
	const char * filename;
	const char * mode;
	PACKFILE * ret;
	if (!lua_checkargs(L, "ss")) goto error;
	filename = lua_tostring(L, 1);
	mode = lua_tostring(L, 2);
	ret = pack_fopen(filename, mode);
	if (!ret) goto error;
	lua_newuserdatabox(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_fclose(lua_State *L)
{
	PACKFILE * f;
	if (!lua_checkargs(L, "u")) goto error;
	f = lua_touserdata(L, 1);
	pack_fclose(f);
	lua_pushnumber(L, 1); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_feof(lua_State *L)
{
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "u")) goto error;
	f = lua_touserdata(L, 1);
	ret = pack_feof(f);
	if (!ret) goto error;
	lua_pushnumber(L, 1); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_getc(lua_State *L)
{
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "u")) goto error;
	f = lua_touserdata(L, 1);
	ret = pack_getc(f);
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_putc(lua_State *L)
{
	int c;
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "nu")) goto error;
	c = lua_tonumber(L, 1);
	f = lua_touserdata(L, 2);
	ret = pack_putc(c, f);
	if (ret == EOF) goto error;
	lua_pushnumber(L, 1); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_igetw(lua_State *L)
{
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "u")) goto error;
	f = lua_touserdata(L, 1);
	ret = pack_igetw(f);
	if (ret == EOF) goto error;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_igetl(lua_State *L)
{
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "u")) goto error;
	f = lua_touserdata(L, 1);
	ret = pack_igetl(f);
	if (ret == EOF) goto error;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_iputw(lua_State *L)
{
	int w;
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "nu")) goto error;
	w = lua_tonumber(L, 1);
	f = lua_touserdata(L, 2);
	ret = pack_iputw(w, f);
	if (ret == EOF) goto error;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_iputl(lua_State *L)
{
	int w;
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "nu")) goto error;
	w = lua_tonumber(L, 1);
	f = lua_touserdata(L, 2);
	ret = pack_iputl(w, f);
	if (ret == EOF) goto error;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

static int bind_pack_fgets(lua_State *L)
{
	PACKFILE * f;
	char buf[4096];
    	const char *ret;
	if (!lua_checkargs(L, "u")) goto error;
	f = lua_touserdata(L, 1);
	ret = pack_fgets(buf, sizeof buf, f);
	if (!ret) goto error;
	lua_pushstring(L, buf); return 1;
error:
	lua_pushnil(L); return 1;    
}

static int bind_pack_fputs(lua_State *L)
{
	const char * p;
	PACKFILE * f;
	int ret;
	if (!lua_checkargs(L, "su")) goto error;
	p = lua_tostring(L, 1);
	f = lua_touserdata(L, 2);
	ret = pack_fputs(p, f);
	if (ret == EOF) goto error;
	lua_pushnumber(L, ret); return 1;
error:
	lua_pushnil(L); return 1;
}

#define DO_PACKFILE_REGISTRATION(L)  \
	lua_register(L, "pack_fopen", bind_pack_fopen); \
	lua_register(L, "pack_fclose", bind_pack_fclose); \
	lua_register(L, "pack_feof", bind_pack_feof); \
	lua_register(L, "pack_getc", bind_pack_getc); \
	lua_register(L, "pack_putc", bind_pack_putc); \
	lua_register(L, "pack_igetw", bind_pack_igetw); \
	lua_register(L, "pack_igetl", bind_pack_igetl); \
	lua_register(L, "pack_iputw", bind_pack_iputw); \
	lua_register(L, "pack_iputl", bind_pack_iputl); \
	lua_register(L, "pack_fgets", bind_pack_fgets); \
	lua_register(L, "pack_fputs", bind_pack_fputs);


/*----------------------------------------------------------------------*/
/*			     The script					*/
/*----------------------------------------------------------------------*/


#include "remap.inc"


/*----------------------------------------------------------------------*/
/*				Main					*/
/*----------------------------------------------------------------------*/

int main (int argc, char *argv[])
{
    lua_State *L;
    int i;

    install_allegro(SYSTEM_NONE, &errno, atexit);

    L = lua_open(0);
    lua_baselibopen(L);
    lua_strlibopen(L);
    lua_iolibopen(L);
    DO_SHIFT_REGISTRATION(L);
    DO_PACKFILE_REGISTRATION(L);
    lua_dostring(L, remap_script);
    
    lua_getglobal(L, "main");
    lua_newtable(L);
    for (i = 0; i < argc; i++) {
	lua_pushnumber(L, i+1);
	lua_pushstring(L, argv[i]);
	lua_settable(L, -3);
    }
    lua_call(L, 1, 0);
    
    return 0;
}

END_OF_MAIN ();
