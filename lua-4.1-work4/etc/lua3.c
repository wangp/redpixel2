/*
** lua3.c
** emulation of Lua 3.2 API on top of Lua 4.0 API
** this code is in the public domain
** bug reports to lua@tecgraf.puc-rio.br
*/

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lua3.h"

#define MAXCB	100			/* maximum nested C blocks */
#define L lua_state			/* let's be lazy from now on */

lua_State* lua_state=NULL;		/* clients need to see this */

typedef struct CStack {
  int lua2C;		/* points to the first element of `array' lua2C */
  int num;		/* size of `array' lua2C */
  int base;		/* points to the first element of `stack' C2Lua */
} CStack;

static CStack CS;
static CStack CB[MAXCB];
static int NCB=0;

/* -- auxiliary functions --------------------------------------------------- */

static void checkspace (void) {
 if (lua_stackspace(L)<=1) lua_error(L,"C stack overflow");
}

/* adjust CS to reflect results from a call to Lua */
static void adjustCS (int base) {
 CS.lua2C=base;				/* position of first arg/result */
 CS.num=lua_gettop(L) - CS.lua2C + 1;	/* number of args/results */
 CS.base=CS.lua2C + CS.num;		/* `limbo' is empty */
}

/*
* wrap a lua_CFunction around a lua3_CFunction.
* lua3_CFunctions do not receive a lua_State, and its upvalues are the
* initial arguments, not the final ones, as in lua_CFunctions.
*/
static int wrapper (lua_State* LL) {
 CStack oldCS=CS;
 int n= (int) lua_tonumber(LL,-1);
 lua3_CFunction fn= (lua3_CFunction) lua_touserdata(LL,-2);
 lua_pop(LL,2);
 while (n--) {
  lua_insert(LL,1);
 }
 adjustCS(1);
 (*fn)();				/* do actual call */
 n=lua_gettop(LL)-CS.base+1;
 CS=oldCS;
 return n;
}

/* -- exported functions start here ----------------------------------------- */

void lua3_open (void) {
 if (L==NULL) {
  L=lua_open(0);
  lua_baselibopen(L);			/* include old builtins */
  CS.lua2C=1;
  CS.num=0;
  CS.base=1;
 }
}

void lua3_close (void) {
 if (L!=NULL) {
  lua_close(L);
  L=NULL;
 }
}

void lua3_beginblock (void) {
 if (NCB>=MAXCB) lua_error(L,"too many nested blocks");
 CB[NCB++]=CS;
}

void lua3_endblock (void) {
 CS=CB[--NCB];
 lua_settop(L,CS.base-1);		/* remove elements from C2Lua */
}

int lua3_dofile (char *filename) {
 CStack oldCS=CS;
 int status;
 lua_settop(L,CS.base-1);		/* remove all elements from lua2C */
 status=lua_dofile(L,filename);
 CS=oldCS;				/* in case of errors */
 adjustCS(CS.base);	/* CS.base is the index of eventual first result */
 return status;
}

int lua3_dostring (char *string) {
 return lua3_dobuffer(string,strlen(string),string);
}

int lua3_dobuffer (char* buff, int size, char* name) {
 CStack oldCS=CS;
 int status;
 lua_settop(L,CS.base-1);		/* remove all elements from lua2C */
 status=lua_dobuffer(L,buff,size,name);
 CS=oldCS;				/* in case of errors */
 adjustCS(CS.base);	/* CS.base is the index of eventual first result */
 return status;
}

void lua3_error (char* s) {
 lua_error(L,s);
}

int lua3_newtag (void) {
 return lua_newtag(L);
}

void lua3_settag (int tag) {
 lua_settag(L,tag);
 lua_pop(L,1);
}

int lua3_tag (lua_Object o) {
 return (o==LUA_NOOBJECT) ? LUA_NOTAG : lua_tag(L,o);
}

int lua3_copytagmethods (int tagto, int tagfrom) {
 return lua_copytagmethods(L,tagto,tagfrom);
}

int lua3_isnil (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_isnil(L,o);
}

int lua3_istable (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_istable(L,o);
}

int lua3_isuserdata (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_isuserdata(L,o);
}

int lua3_iscfunction (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_iscfunction(L,o);
}

int lua3_isnumber (lua_Object o) {
 return lua_isnumber(L,o);
}

int lua3_isstring (lua_Object o) {
 return lua_isstring(L,o);
}

int lua3_isfunction (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_isfunction(L,o);
}

double lua3_getnumber (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_tonumber(L,o);
}

char* lua3_getstring (lua_Object o) {
 return (o==LUA_NOOBJECT) ? NULL : (char*) lua_tostring(L,o);
}

long lua3_strlen (lua_Object o) {
 return (o==LUA_NOOBJECT) ? 0 : lua_strlen(L,o);
}

void* lua3_getuserdata (lua_Object o) {
 return (o==LUA_NOOBJECT) ? NULL : lua_touserdata(L,o);
}

void lua3_pushnil (void) {
 checkspace();
 lua_pushnil(L);
}

void lua3_pushnumber (double n) {
 checkspace();
 lua_pushnumber(L,n);
}

void lua3_pushlstring (char* s, long len) {
 checkspace();
 lua_pushlstring(L,s,len);
}

void lua3_pushstring (char* s) {
 checkspace();
 lua_pushstring(L,s);
}

void lua3_pushobject (lua_Object o) {
 checkspace();
 lua_pushvalue(L,o);
}

void lua3_pushcclosure (lua3_CFunction fn, int n) {
 checkspace();
 lua_pushuserdata(L,(void*)fn);		/* dangerous cast! */
 lua_pushnumber(L,n);
 lua_pushcclosure(L,wrapper,n+2);
}

void lua3_pushusertag (void* u, int tag) {
 checkspace();
 lua_pushusertag(L,u,tag);
}

/* put top element into `limbo' */
lua_Object lua3_pop (void) {
  lua_insert(L,CS.base);
  return CS.base++;
}

lua_Object lua3_lua2C (int n) {
  return (n<= 0 || n>CS.num) ? LUA_NOOBJECT : CS.lua2C+n-1;
}

lua_Object lua3_getglobal (char* name) {
 lua_getglobal(L,name);
 return lua3_pop();
}

void lua3_setglobal (char* name) {
 lua_setglobal(L,name);
}

lua_Object lua3_rawgetglobal (char* name) {
 lua_getglobals(L);
 lua_pushstring(L,name);
 lua_rawget(L,-2);
 lua_remove(L,-2);
 return lua3_pop();
}

void lua3_rawsetglobal (char* name) {
 lua_getglobals(L);
 lua_pushstring(L,name);
 lua_pushvalue(L,-3);
 lua_rawset(L,-3);
 lua_pop(L,2);
}

void lua3_settable (void) {
 lua_settable(L,-3);
 lua_pop(L,1);
}

lua_Object lua3_gettable (void) {
 lua_gettable(L,-2);
 lua_remove(L,-2);
 return lua3_pop();
}

void lua3_rawsettable (void) {
 lua_rawset(L,-3);
 lua_pop(L,1);
}

lua_Object lua3_rawgettable (void) {
 lua_rawget(L,-2);
 lua_remove(L,-2);
 return lua3_pop();
}

int lua3_ref (int lock) {
 return lua_ref(L,lock);
}

lua_Object lua3_getref (int ref) {
 return (lua_getref(L,ref)==0) ? LUA_NOOBJECT : lua3_pop();
}

void lua3_unref (int ref) {
 lua_unref(L,ref);
}

lua_Object lua3_createtable (void) {
 lua_newtable(L);
 return lua3_pop();
}

int lua3_callfunction (lua_Object o) {
 if (o==LUA_NOOBJECT)
  return 1;
 else {
  CStack oldCS=CS;
  int status;
  lua_pushvalue(L,o);
  lua_insert(L,CS.base);  /* move function to proper position */
  status=lua_call(L,lua_gettop(L) - CS.base, LUA_MULTRET);
  CS=oldCS;
  adjustCS(CS.base);
  return status;
 }
}

lua_Object lua3_seterrormethod (void) {
 lua_Object old=lua3_getglobal(LUA_ERRORMESSAGE);
 lua3_setglobal(LUA_ERRORMESSAGE);
 return old;
}

lua_Object lua3_settagmethod (int tag, char* event) {
 lua_Object old=lua3_gettagmethod(tag,event);
 lua_settagmethod(L,tag,event);
 return old;
}

lua_Object lua3_gettagmethod (int tag, char* event) {
 lua_gettagmethod(L,tag,event);
 return lua3_pop();
}

/* -- these are not ready yet ------------------------------------------------*/

long lua3_collectgarbage (long limit) {
 lua_setgcthreshold(L,0);
 return 0;
}

lua3_CFunction lua3_getcfunction (lua_Object o) {
 lua_error(L,"lua3_getcfunction not implemented yet");
 return NULL;
}

/* -- an implementation of lauxlib.c ---------------------------------------- */

void lua3L_openlib (struct lua3L_reg *l, int n) {
 int i;
 lua3_open();
 for (i=0; i<n; i++) {
  lua3_pushcclosure(l[i].func,0);
  lua3_setglobal(l[i].name);
 }
}

void lua3L_verror (char *fmt, ...) {
 char buff[500];
 va_list argp;
 va_start(argp, fmt);
 vsprintf(buff, fmt, argp);
 va_end(argp);
 lua3_error(buff);
}

char *lua3L_check_lstr (int numArg, long *len) {
 size_t l;
 const char *s=luaL_check_lstr(L,numArg,&l);
 if (len!=NULL) *len=l;
 return (char*) s;
}

char *lua3L_opt_lstr (int numArg, char *def, long *len) {
 size_t l;
 const char *s=luaL_opt_lstr(L,numArg,def,&l);
 if (len!=NULL) *len=l;
 return (char*) s;
}

