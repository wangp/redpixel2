/*
** lua3.h
** emulation of Lua 3.2 API on top of Lua 4.0 API
** this code is in the public domain
** bug reports to lua@tecgraf.puc-rio.br
*/

#ifndef lua3_h
#define lua3_h

#include <stddef.h>			/* definition of `size_t' */

#define LUA_NOOBJECT  0
#define LUA_ANYTAG    (-1)

typedef void (*lua3_CFunction) (void);
typedef unsigned int lua_Object;

struct lua3L_reg {
  char *name;
  lua3_CFunction func;
};

#ifndef lua_h

typedef struct lua_State lua_State;
extern lua_State *lua_state;

#define lua_CFunction		lua3_CFunction
#define lua_beginblock		lua3_beginblock
#define lua_callfunction	lua3_callfunction
#define lua_close		lua3_close
#define lua_collectgarbage	lua3_collectgarbage
#define lua_copytagmethods	lua3_copytagmethods
#define lua_createtable		lua3_createtable
#define lua_dobuffer		lua3_dobuffer
#define lua_dofile		lua3_dofile
#define lua_dostring		lua3_dostring
#define lua_endblock		lua3_endblock
#define lua_error		lua3_error
#define lua_getcfunction	lua3_getcfunction
#define lua_getglobal		lua3_getglobal
#define lua_getnumber		lua3_getnumber
#define lua_getref		lua3_getref
#define lua_getstring		lua3_getstring
#define lua_gettable		lua3_gettable
#define lua_gettagmethod	lua3_gettagmethod
#define lua_getuserdata		lua3_getuserdata
#define lua_iscfunction		lua3_iscfunction
#define lua_isfunction		lua3_isfunction
#define lua_isnil		lua3_isnil
#define lua_isnumber		lua3_isnumber
#define lua_isstring		lua3_isstring
#define lua_istable		lua3_istable
#define lua_isuserdata		lua3_isuserdata
#define lua_lua2C		lua3_lua2C
#define lua_newtag		lua3_newtag
#define lua_open		lua3_open
#define lua_pop			lua3_pop
#define lua_pushcclosure	lua3_pushcclosure
#define lua_pushlstring		lua3_pushlstring
#define lua_pushnil		lua3_pushnil
#define lua_pushnumber		lua3_pushnumber
#define lua_pushobject		lua3_pushobject
#define lua_pushstring		lua3_pushstring
#define lua_pushusertag		lua3_pushusertag
#define lua_rawgetglobal	lua3_rawgetglobal
#define lua_rawgettable		lua3_rawgettable
#define lua_rawsetglobal	lua3_rawsetglobal
#define lua_rawsettable		lua3_rawsettable
#define lua_ref			lua3_ref
#define lua_seterrormethod	lua3_seterrormethod
#define lua_setglobal		lua3_setglobal
#define lua_settable		lua3_settable
#define lua_settag		lua3_settag
#define lua_settagmethod	lua3_settagmethod
#define lua_strlen		lua3_strlen
#define lua_tag			lua3_tag
#define lua_unref		lua3_unref

#define lua_getparam(_)		lua_lua2C(_)
#define lua_getresult(_)	lua_lua2C(_)
#define lua_call(name)		lua_callfunction(lua_getglobal(name))
#define lua_pushref(ref)	lua_pushobject(lua_getref(ref))
#define lua_refobject(o,l)	(lua_pushobject(o), lua_ref(l))
#define lua_register(n,f)	(lua_pushcfunction(f), lua_setglobal(n))
#define lua_pushuserdata(u)     lua_pushusertag(u, 0)
#define lua_pushcfunction(f)	lua_pushcclosure(f, 0)
#define lua_clonetag(t)		lua_copytagmethods(lua_newtag(), (t))

/* from 4.0 lualib.h */
void lua_iolibopen (lua_State *L);
void lua_strlibopen (lua_State *L);
void lua_mathlibopen (lua_State *L);
void lua_dblibopen (lua_State *L);

#define lua_iolibopen()		(lua_iolibopen)(lua_state)
#define lua_strlibopen()	(lua_strlibopen)(lua_state)
#define lua_mathlibopen()	(lua_mathlibopen)(lua_state)
#define lua_dblibopen()		(lua_dblibopen)(lua_state)

/* from 4.0 lauxlib.h */
double luaL_check_number (lua_State *L, int numArg);
double luaL_opt_number (lua_State *L, int numArg, double def);
void luaL_argerror (lua_State *L, int numarg, const char *extramsg);
const char *luaL_check_lstr (lua_State *L, int numArg, size_t *len);
const char *luaL_opt_lstr (lua_State *L, int numArg, const char *def, size_t *len);

#define luaL_reg		lua3L_reg
#define luaL_openlib		lua3L_openlib
#define luaL_verror		lua3L_verror
#define luaL_check_lstr		lua3L_check_lstr
#define luaL_opt_lstr		lua3L_opt_lstr

#define luaL_arg_check(c,n,m)	if (!(c)) luaL_argerror(n,m)
#define luaL_argerror(n,m)	(luaL_argerror)(lua_state,n,m)
#define luaL_check_string(n)	luaL_check_lstr(n,NULL)
#define luaL_opt_string(n,d)	luaL_opt_lstr(n,d,NULL)
#define luaL_check_number(n)	(luaL_check_number)(lua_state,n)
#define luaL_opt_number(n,d)	(luaL_opt_number)(lua_state,n,d)
#define luaL_check_int(n)	((int)luaL_check_number(n))
#define luaL_opt_int(n,d)	((int)luaL_opt_number(n,d))
#define luaL_check_long(n)	((long)luaL_check_number(n))
#define luaL_opt_long(n,d)	((long)luaL_opt_number(n,d))

#endif

void lua3_beginblock (void);
int lua3_callfunction (lua_Object f);
void lua3_close (void);
long lua3_collectgarbage (long limit);
int lua3_copytagmethods (int tagto, int tagfrom);
lua_Object lua3_createtable (void);
int lua3_dobuffer (char* buff, int size, char* name);
int lua3_dofile (char* filename);
int lua3_dostring (char* string);
void lua3_endblock (void);
void lua3_error (char* s);
lua3_CFunction lua3_getcfunction (lua_Object object);
lua_Object lua3_getglobal (char* name);
double lua3_getnumber (lua_Object object);
lua_Object lua3_getref (int ref);
char* lua3_getstring (lua_Object object);
lua_Object lua3_gettable (void);
lua_Object lua3_gettagmethod (int tag, char* event);
void* lua3_getuserdata (lua_Object object);
int lua3_iscfunction (lua_Object object);
int lua3_isfunction (lua_Object object);
int lua3_isnil (lua_Object object);
int lua3_isnumber (lua_Object object);
int lua3_isstring (lua_Object object);
int lua3_istable (lua_Object object);
int lua3_isuserdata (lua_Object object);
lua_Object lua3_lua2C (int number);
int lua3_newtag (void);
void lua3_open (void);
lua_Object lua3_pop (void);
void lua3_pushcclosure (lua3_CFunction fn, int n);
void lua3_pushlstring (char* s, long len);
void lua3_pushnil (void);
void lua3_pushnumber (double n);
void lua3_pushobject (lua_Object object);
void lua3_pushstring (char* s);
void lua3_pushusertag (void* u, int tag);
lua_Object lua3_rawgetglobal (char* name);
lua_Object lua3_rawgettable (void);
void lua3_rawsetglobal (char* name);
void lua3_rawsettable (void);
int lua3_ref (int lock);
lua_Object lua3_seterrormethod (void);
void lua3_setglobal (char* name);
void lua3_settable (void);
void lua3_settag (int tag);
lua_Object lua3_settagmethod (int tag, char* event);
long lua3_strlen (lua_Object object);
int lua3_tag (lua_Object object);
void lua3_unref (int ref);

void lua3L_openlib (struct lua3L_reg *l, int n);
void lua3L_verror (char *fmt, ...);
char *lua3L_check_lstr (int numArg, long *len);
char *lua3L_opt_lstr (int numArg, char *def, long *len);

#endif
