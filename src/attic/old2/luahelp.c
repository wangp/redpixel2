/* luahelp.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <lua.h>
#include "luahelp.h"


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
	default: return 1;
    }

    return (lua_type (L, index) == type);
}

int lua_checkargs (lua_State *L, const char *argtypes)
{
    const char *a = argtypes;
    int index = 0;

    for (a = argtypes; *a; a++, index++)
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

