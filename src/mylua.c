/* mylua.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <lua.h>
#include <lualib.h>
#include "mylua.h"
#include "path.h"


lua_State *lua_state;


int mylua_open (int stacksize)
{
    if (!(lua_state = lua_open (stacksize)))
	return -1;
    lua_baselibopen (lua_state);
    return 0;
}


void mylua_close ()
{
    lua_close (lua_state);
}


int lua_dofile_path (lua_State *L, const char *filename)
{
    char **p, tmp[1024];

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filename, sizeof tmp);
	if (lua_dofile (L, tmp) == 0)
	    return 0;
    }

    return -1;
}


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
