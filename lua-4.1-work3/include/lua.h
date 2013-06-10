/*
** $Id: lua.h,v 1.111 2001/12/11 22:48:44 roberto Exp $
** Lua - An Extensible Extension Language
** TeCGraf: Grupo de Tecnologia em Computacao Grafica, PUC-Rio, Brazil
** e-mail: info@lua.org
** www: http://www.lua.org
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h


/* definition of `size_t' */
#include <stddef.h>



#define LUA_VERSION	"Lua 4.1 (work3)"
#define LUA_COPYRIGHT	"Copyright (C) 1994-2001 TeCGraf, PUC-Rio"
#define LUA_AUTHORS 	"W. Celes, R. Ierusalimschy & L. H. de Figueiredo"


/* name of global variable with error handler */
#define LUA_ERRORMESSAGE	"_ERRORMESSAGE"


/* option for multiple returns in `lua_call' */
#define LUA_MULTRET	(-1)


/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_GLOBALSINDEX	(-10001)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/* error codes for `lua_do*' and the like */
#define LUA_ERRRUN	1
#define LUA_ERRFILE	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5

/* weak-table modes */
#define LUA_WEAK_KEY	1
#define LUA_WEAK_VALUE	2


typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);



/*
** basic types
*/
#define LUA_TNONE	(-1)

#define LUA_TUSERDATA	0
#define LUA_TNIL	1
#define LUA_TNUMBER	2
#define LUA_TBOOLEAN	3
#define LUA_TSTRING	4
#define LUA_TTABLE	5
#define LUA_TFUNCTION	6


/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/*
** generic extra include file
*/
#ifdef LUA_USER_H
#include LUA_USER_H
#endif


/* Lua numerical type */
#ifndef LUA_NUMBER
#define LUA_NUMBER	double
#endif
typedef LUA_NUMBER lua_Number;


/* mark for all API functions */
#ifndef LUA_API
#define LUA_API		extern
#endif


/*
** state manipulation
*/
LUA_API lua_State *lua_newthread (lua_State *L, int stacksize);
LUA_API void       lua_close (lua_State *L);


/*
** basic stack manipulation
*/
LUA_API int   lua_gettop (lua_State *L);
LUA_API void  lua_settop (lua_State *L, int index);
LUA_API void  lua_pushvalue (lua_State *L, int index);
LUA_API void  lua_remove (lua_State *L, int index);
LUA_API void  lua_insert (lua_State *L, int index);
LUA_API int   lua_stackspace (lua_State *L);


/*
** access functions (stack -> C)
*/

LUA_API int             lua_isnumber (lua_State *L, int index);
LUA_API int             lua_istrue (lua_State *L, int index);
LUA_API int             lua_isstring (lua_State *L, int index);
LUA_API int             lua_iscfunction (lua_State *L, int index);
LUA_API int             lua_type (lua_State *L, int index);
LUA_API const char     *lua_typename (lua_State *L, int type);

LUA_API int            lua_equal (lua_State *L, int index1, int index2);
LUA_API int            lua_lessthan (lua_State *L, int index1, int index2);

LUA_API lua_Number      lua_tonumber (lua_State *L, int index);
LUA_API int             lua_toboolean (lua_State *L, int index);
LUA_API const char *lua_tostring (lua_State *L, int index);
LUA_API size_t          lua_strlen (lua_State *L, int index);
LUA_API lua_CFunction   lua_tocfunction (lua_State *L, int index);
LUA_API void	       *lua_touserdata (lua_State *L, int index);
LUA_API const void     *lua_topointer (lua_State *L, int index);


/*
** push functions (C -> stack)
*/
LUA_API void  lua_pushnil (lua_State *L);
LUA_API void  lua_pushnumber (lua_State *L, lua_Number n);
LUA_API void  lua_pushlstring (lua_State *L, const char *s, size_t len);
LUA_API void  lua_pushstring (lua_State *L, const char *s);
LUA_API void  lua_pushcclosure (lua_State *L, lua_CFunction fn, int n);
LUA_API void  lua_pushboolean (lua_State *L, int b);


/*
** get functions (Lua -> stack)
*/
LUA_API void  lua_getstr (lua_State *L, int index, const char *name);
LUA_API void  lua_gettable (lua_State *L, int index);
LUA_API void  lua_rawget (lua_State *L, int index);
LUA_API void  lua_rawgeti (lua_State *L, int index, int n);
LUA_API void  lua_newtable (lua_State *L);
LUA_API void  lua_getweakregistry (lua_State *L);
LUA_API void  lua_geteventtable (lua_State *L, int objindex);


/*
** set functions (stack -> Lua)
*/
LUA_API void  lua_setstr (lua_State *L, int index, const char *name);
LUA_API void  lua_settable (lua_State *L, int index);
LUA_API void  lua_rawset (lua_State *L, int index);
LUA_API void  lua_rawseti (lua_State *L, int index, int n);
LUA_API void  lua_setglobals (lua_State *L);
LUA_API void  lua_seteventtable (lua_State *L, int objindex);


/*
** `load' and `do' functions (load and run Lua code)
*/
LUA_API int   lua_call (lua_State *L, int nargs, int nresults);
LUA_API void  lua_rawcall (lua_State *L, int nargs, int nresults);
LUA_API int   lua_loadfile (lua_State *L, const char *filename);
LUA_API int   lua_dofile (lua_State *L, const char *filename);
LUA_API int   lua_dostring (lua_State *L, const char *str);
LUA_API int   lua_loadbuffer (lua_State *L, const char *buff, size_t size,
                            const char *name);
LUA_API int   lua_dobuffer (lua_State *L, const char *buff, size_t size,
                            const char *name);

/*
** Garbage-collection functions
*/
LUA_API int   lua_getgcthreshold (lua_State *L);
LUA_API int   lua_getgccount (lua_State *L);
LUA_API void  lua_setgcthreshold (lua_State *L, int newthreshold);

/*
** miscellaneous functions
*/

LUA_API void  lua_error (lua_State *L, const char *s);

LUA_API int   lua_next (lua_State *L, int index);
LUA_API int   lua_getn (lua_State *L, int index);

LUA_API void  lua_concat (lua_State *L, int n);

LUA_API void *lua_newuserdata (lua_State *L, size_t size);
LUA_API void  lua_newuserdatabox (lua_State *L, void *u);

LUA_API void  lua_setweakmode (lua_State *L, int mode);
LUA_API int   lua_getweakmode (lua_State *L, int index);



/* 
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_open(n)		lua_newthread(NULL, (n))

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_register(L,n,f)	(lua_pushcfunction(L, f), lua_setglobal(L, n))
#define lua_pushcfunction(L,f)	lua_pushcclosure(L, f, 0)

#define lua_isfunction(L,n)	(lua_type(L,n) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L,n) == LUA_TTABLE)
#define lua_isuserdata(L,n)	(lua_type(L,n) == LUA_TUSERDATA)
#define lua_isnil(L,n)		(lua_type(L,n) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L,n) == LUA_TBOOLEAN)
#define lua_isnull(L,n)		(lua_type(L,n) == LUA_TNONE)

#define lua_pushliteral(L, s)	lua_pushlstring(L, "" s, \
                                                (sizeof(s)/sizeof(char))-1)

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)
#define lua_getglobals(L)	lua_pushvalue(L, LUA_GLOBALSINDEX)
#define lua_getglobal(L,s)	lua_getstr(L, LUA_GLOBALSINDEX, s)
#define lua_setglobal(L,s)	lua_setstr(L, LUA_GLOBALSINDEX, s)



/*
** compatibility macros and functions
*/

LUA_API void lua_pushupvalues (lua_State *L);


/* compatibility with ref system */

/* pre-defined references */
#define LUA_NOREF	(-2)
#define LUA_REFNIL	(-1)

#define lua_ref(L,lock)	((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
		(lua_error(L, "unlocked references are obsolete"), 0))

#define lua_unref(L,ref)	luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)	lua_rawgeti(L, LUA_REGISTRYINDEX, ref)

#endif



/*
** {======================================================================
** useful definitions for Lua kernel and libraries
** =======================================================================
*/

/* formats for Lua numbers */
#ifndef LUA_NUMBER_SCAN
#define LUA_NUMBER_SCAN		"%lf"
#endif

#ifndef LUA_NUMBER_FMT
#define LUA_NUMBER_FMT		"%.16g"
#endif

/* function to convert a lua_Number to a string */
#ifndef lua_number2str
#define lua_number2str(s,n)     sprintf((s), LUA_NUMBER_FMT, (n))
#endif

/* function to convert a string to a lua_Number */
#ifndef lua_str2number
#define lua_str2number(s,p)     strtod((s), (p))
#endif

/* }====================================================================== */



/******************************************************************************
* Copyright (C) 1994-2001 TeCGraf, PUC-Rio.  All rights reserved.
* 
* Permission is hereby granted, without written agreement and without license
* or royalty fees, to use, copy, modify, and distribute this software and its
* documentation for any purpose, including commercial applications, subject to
* the following conditions:
* 
*  - The above copyright notice and this permission notice shall appear in all
*    copies or substantial portions of this software.
* 
*  - The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software in a
*    product, an acknowledgment in the product documentation would be greatly
*    appreciated (but it is not required).
* 
*  - Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
*    
* The authors specifically disclaim any warranties, including, but not limited
* to, the implied warranties of merchantability and fitness for a particular
* purpose.  The software provided hereunder is on an "as is" basis, and the
* authors have no obligation to provide maintenance, support, updates,
* enhancements, or modifications.  In no event shall TeCGraf, PUC-Rio, or the
* authors be held liable to any party for direct, indirect, special,
* incidental, or consequential damages arising out of the use of this software
* and its documentation.
* 
* The Lua language and this implementation have been entirely designed and
* written by Waldemar Celes Filho, Roberto Ierusalimschy and
* Luiz Henrique de Figueiredo at TeCGraf, PUC-Rio.
*
* This implementation contains no third-party code.
******************************************************************************/

