/* ddl.c -- configuration module for lua.c; see config.c
*
* This is a bare-bones dynamic library loader for Lua under Windows.
* See loadlib.c for an explanation of how it works externally.
*/

#define LUA_EXTRALIBS	{"loadlib", register_loadlib},

#include <windows.h>

static void pusherror(lua_State *L)
{
 int error=GetLastError();
 char buffer[128];
 if (FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	0, error, 0, buffer, sizeof(buffer), 0))
  lua_pushstring(L,buffer);
 else
  lua_pushfstring("system error %d\n",error);
}

static int loadlib(lua_State *L)		/* loadlib(path,init) */
{
 const char *path=luaL_checkstring(L,1);
 const char *init=luaL_checkstring(L,2);
 HINSTANCE lib=LoadLibrary(path);
 if (lib!=NULL)
 {
  lua_CFunction f=(lua_CFunction) GetProcAddress(lib,init);
  if (f!=NULL)
  {
   lua_pushlightuserdata(L,lib);
   lua_pushcclosure(L,f,1);
   return 1;
  }
 }
 lua_pushnil(L);
 pusherror(L);
 lua_pushstring(L,(lib!=NULL) ? "init" : "open");
 if (lib!=NULL) FreeLibrary(lib);
 return 3;
}

static int register_loadlib(lua_State *L)
{
 lua_register(L,"loadlib",loadlib);
 return 0;
}
