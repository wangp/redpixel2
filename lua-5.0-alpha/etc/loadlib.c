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
* nil and  two strings describing  the error.  The first string  is simply
* either "open"  or "init"  to identify  the error;  the second  string is
* supplied by the operating system and should be more informative.
* 
* This bare-bones loadlib is supposed to  be used as a foundation for more
* sophisticated dynamic  library loaders,  possibly still  called loadlib,
* that  will be  smart enough  to  try to  find the  library in  different
* directories and also to guess the name of the init function.
*/

#define lua_userinit(L)		openstdlibs(L)+register_loadlib(L)

#include <dlfcn.h>

static int loadlib(lua_State *L)		/* loadlib(path,init) */
{
 const char *path=luaL_check_string(L,1);
 const char *init=luaL_check_string(L,2);
 void *lib=dlopen(path,RTLD_NOW);
 if (lib!=NULL)
 {
  lua_CFunction f=(lua_CFunction) dlsym(lib,init);
  if (f!=NULL)
  {
   lua_pushcclosure(L,f,0);
   return 1;
  }
 }
 lua_pushnil(L);
 lua_pushstring(L,(lib!=NULL) ? "init" : "open");
 lua_pushstring(L,dlerror());
 if (lib!=NULL) dlclose(lib);
 return 3;
}

static int register_loadlib(lua_State *L)
{
 lua_register(L,"loadlib",loadlib);
 return 0;
}
