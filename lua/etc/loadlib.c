/* loadlib.c -- configuration module for lua.c; see config.c
*
* This is a bare-bones dynamic library loader for Lua based on the dlfcn
* interface. This interface is available in Linux, SunOS, Solaris, AIX 4.2,
* HPUX 11, IRIX, FreeBSD, NetBSD, and probably others.
* 
* This Lua  library exports  a single function,  called loadlib,  which is
* called from Lua as loadlib(lib,init), where  lib is the full name of the
* library to be loaded (including the  complete path) and init is the name
* of a function to be called  after the library is loaded. Typically, this
* function will register other functions, thus making the complete library
* available  to Lua.  The  init function  is  *not* automatically  called.
* Instead, loadlib  returns the init function  and the client can  call it
* when it  thinks is appropriate. In  the case of errors,  loadlib returns
* nil and two  strings describing the error. The first  string is supplied
* by the operating  system; it should be informative and  useful for error
* messages.  The  second string  is  simply  either  "open" or  "init"  to
* identify the error and is meant  to be used for making decisions without
* having to look into the first string (whose format is system-dependent).
* 
* This bare-bones loadlib is supposed to  be used as a foundation for more
* sophisticated dynamic library loaders, possibly still called loadlib and
* probably written in  Lua, that will be  smart enough to try  to find the
* library in different directories and also  perhaps guess the name of the
* init function.
*/

#define LUA_EXTRALIBS	{"loadlib", register_loadlib},

#include <dlfcn.h>

static int loadlib(lua_State *L)		/* loadlib(path,init) */
{
 const char *path=luaL_checkstring(L,1);
 const char *init=luaL_checkstring(L,2);
 void *lib=dlopen(path,RTLD_NOW);
 if (lib!=NULL)
 {
  lua_CFunction f=(lua_CFunction) dlsym(lib,init);
  if (f!=NULL)
  {
   lua_pushlightuserdata(L,lib);
   lua_pushcclosure(L,f,1);
   return 1;
  }
 }
 lua_pushnil(L);
 lua_pushstring(L,dlerror());
 lua_pushstring(L,(lib!=NULL) ? "init" : "open");
 if (lib!=NULL) dlclose(lib);
 return 3;
}

static int register_loadlib(lua_State *L)
{
 lua_register(L,"loadlib",loadlib);
 return 0;
}
