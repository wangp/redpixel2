/* mylua.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <time.h>
#include <allegro.h>
#include "lua.h"
#include "lualib.h"
#include "mylua.h"
#include "path.h"


/*
 * Import bindings.
 */

#include "client.h"
#include "explo.h"
#include "extdata.h"
#include "loaddata.h"
#include "svgame.h"
#include "store.h"
#include "object.h"
#include "objtypes.h"


#include "bindings.inc"



/*
 * The actual module stuff.
 */


/* We use one Lua state.  But we don't want clients using functions
 * which only make sense in a server context, and vice versa, i.e. we
 * want namespaces.  To do this, we create an initial state with some
 * globals needed for initialisation registered (e.g. data loading
 * functions).  Afterwards we split the state into two "threads" with
 * different globals tables, then register some more functions.  */

lua_State *the_lua_state;
lua_State *server_lua_namespace;
lua_State *client_lua_namespace;


int mylua_open (void)
{
    the_lua_state = lua_open ();
    if (!the_lua_state)
	return -1;
    server_lua_namespace = NULL;
    client_lua_namespace = NULL;

    lua_baselibopen (the_lua_state);
    lua_mathlibopen (the_lua_state); /* XXX: needed? */
    lua_strlibopen (the_lua_state);

    /* seed RNG */
    lua_getglobal (the_lua_state, "randomseed");
    lua_pushnumber (the_lua_state, time (0));
    lua_call (the_lua_state, 1, 0);

    DO_REGISTRATION_BOTH (the_lua_state);

    return 0;
}


static lua_State *fork_namespace (lua_State *orig_state)
{
    lua_State *new_state;
    lua_ref_t table;

    new_state = lua_newthread (orig_state);

    /* Make a copy of orig_state's globals table on top of the
     * orig_state stack.  */
    lua_dostring (orig_state, "
	do
	  local t = {}
	  for i,v in globals () do
	    t[i] = v
	  end
	  return t
	end
    ");

    /* Make the new table new_state's global table.  */
    table = lua_ref (orig_state, 1);
    lua_getref (new_state, table);
    lua_setglobals (new_state);
    lua_unref (orig_state, table);

    return new_state;
}


void mylua_open_server_and_client_namespaces (void)
{
    server_lua_namespace = fork_namespace (the_lua_state);
    DO_REGISTRATION_SERVER (server_lua_namespace);

    client_lua_namespace = fork_namespace (the_lua_state);
    DO_REGISTRATION_CLIENT (client_lua_namespace);

    /* for debugging */
    lua_pushstring (the_lua_state, "initial");
    lua_setglobal (the_lua_state, "_internal_namespace");

    lua_pushstring (server_lua_namespace, "server");
    lua_setglobal (server_lua_namespace, "_internal_namespace");

    lua_pushstring (client_lua_namespace, "client");
    lua_setglobal (client_lua_namespace, "_internal_namespace");
}


void mylua_close (void)
{
    if (client_lua_namespace) {
	lua_closethread (the_lua_state, client_lua_namespace);
	client_lua_namespace = NULL;
    }

    if (server_lua_namespace) {
	lua_closethread (the_lua_state, server_lua_namespace);
	server_lua_namespace = NULL;
    }

    lua_close (the_lua_state);
    the_lua_state = NULL;
}


int lua_dofile_path (lua_State *L, const char *filename)
{
    char **p, tmp[1024];

    for (p = path_share; *p; p++) {
	ustrzcpy (tmp, sizeof tmp, *p);
	ustrzcat (tmp, sizeof tmp, filename);
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
