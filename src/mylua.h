#ifndef __included_mylua_h
#define __included_mylua_h


#include <lua.h>


typedef int lref_t;
typedef int ltag_t;

#define LS	lua_State *L

extern lua_State *lua_state;

int mylua_open (int stacksize);
void mylua_close ();

void lua_dofiles (lua_State *L, const char *filenames);
int lua_checkargs (lua_State *L, const char *argtypes);


/* Macros to take away the prefix.  */

/*
** basic stack manipulation
*/
#define gettop(L)			(lua_gettop (L))
#define settop(L, index)		(lua_settop (L, index))
#define pushvalue(L, index)		(lua_pushvalue (L, index))
#define lremove(L, index)		(lua_remove (L, index))
#define insert(L, index)		(lua_insert (L, index))
#define stackspace(L)			(lua_stackspace (L))


/*
** access functions (stack -> C)
*/

#define type(L, index)		  	(lua_type (L, index))
#define ltypename(L, t)		  	(lua_typename (L, t))
#define isnumber(L, index)		(lua_isnumber (L, index))
#define isstring(L, index)		(lua_isstring (L, index))
#define iscfunction(L, index)	  	(lua_iscfunction (L, index))
#define tag(L, index)		  	(lua_tag (L, index))

#define equal(L, index1, index2)	(lua_equal (L, index1, index2))
#define lessthan(L, index1, index2)  	(lua_lessthan (L, index1, index2))

#define tonumber(L, index)		(lua_tonumber (L, index))
#define tostring(L, index)		(lua_tostring (L, index))
#define lstrlen(L, index)		(lua_strlen (L, index))
#define tocfunction(L, index)	  	(lua_tocfunction (L, index))
#define touserdata(L, index)	  	(lua_touserdata (L, index))
#define topointer(L, index)	  	(lua_topointer (L, index))


/*
** push functions (C -> stack)
*/
#define pushnil(L)		  	(lua_pushnil (L))
#define pushnumber(L, n)		(lua_pushnumber (L, n))
#define pushlstring(L, s, len)	  	(lua_pushlstring (L, s, len))
#define pushstring(L, s)		(lua_pushstring (L, s))
#define pushcclosure(L, fn, n)	  	(lua_pushcclosure (L, fn, n))
#define pushusertag(L, u, tag)	  	(lua_pushusertag (L, u, tag))


/*
** get functions (Lua -> stack)
*/
#define getglobal(L, name)		(lua_getglobal (L, name))
#define gettable(L, index)		(lua_gettable (L, index))
#define rawget(L, index)		(lua_rawget (L, index))
#define rawgeti(L, index, n)	  	(lua_rawgeti (L, index, n))
#define getglobals(L)		  	(lua_getglobals (L))
#define gettagmethod(L, tag, event)  	(lua_gettagmethod (L, tag, event))
#define getref(L, ref)		  	(lua_getref (L, ref))
#define newtable(L)		  	(lua_newtable (L))


/*
** set functions (stack -> Lua)
*/
#define setglobal(L, name)		(lua_setglobal (L, name))
#define settable(L, index)		(lua_settable (L, index))
#define rawset(L, index)		(lua_rawset (L, index))
#define rawseti(L, index, n)	  	(lua_rawseti (L, index, n))
#define setglobals(L)		  	(lua_setglobals (L))
#define settagmethod(L, tag, event)  	(lua_settagmethod (L, tag, event))
#define ref(L, lock)		  	(lua_ref (L, lock))


/*
** "do" functions (run Lua code)
*/
#define call(L, nargs, nresults)	(lua_call (L, nargs, nresults))
#define rawcall(L, nargs, nresults)	(lua_rawcall (L, nargs, nresults))
#define dofile(L, filename)	  	(lua_dofile (L, filename))
#define dostring(L, str)		(lua_dostring (L, str))
#define dobuffer(L, buf, sz, name)	(lua_dobuffer (L, buf, sz, name))


/*
** miscellaneous functions
*/
#define newtag(L)		  	(lua_newtag (L))
#define settag(L, tag)		  	(lua_settag (L, tag))

#define unref(L, ref)		  	(lua_unref (L, ref))

#define next(L, index)		  	(lua_next (L, index))
#define getn(L, index)		  	(lua_getn (L, index))


/* 
** some useful macros
*/

#define pop(L, index)			(lua_pop (L, index))

#define lregister(L, n, f)		(lua_register (L, n, f))
#define pushuserdata(L, u)		(lua_pushuserdata (L, u))
#define pushcfunction(L, f)		(lua_pushcfunction (L, f))

#define isfunction(L, index)		(lua_isfunction (L, index))
#define istable(L, index)		(lua_istable (L, index))
#define isuserdata(L, index)		(lua_isuserdata (L, index))
#define isnil(L, index)			(lua_isnil (L, index))
#define isnull(L, index)		(lua_isnull (L, index))


/* 
** my stuff
*/

#define dofiles(L, filenames)		(lua_dofiles (L, filenames))
#define checkargs(L, argtypes)		(lua_checkargs (L, argtypes))


#endif
