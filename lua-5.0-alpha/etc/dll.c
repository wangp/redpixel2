/* ddl.c -- configuration module for lua.c; see config.c and loadlib.c
* a bare-bones dynamic library loader for Lua under Windows
*/

#define lua_userinit(L)		openstdlibs(L)+register_loadlib(L)

#include <stdio.h>
#include <windows.h>

static void pusherror(lua_State *L)
{
 char buffer[512];
 if (!FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
	0, GetLastError(), 0, buffer, sizeof(buffer), 0))
   sprintf(buffer,"error %d\n",GetLastError());
 lua_pushstring(L,buffer);
}

static int loadlib(lua_State *L)		/* loadlib(path,init) */
{
 const char *path=luaL_check_string(L,1);
 const char *init=luaL_check_string(L,2);
 HINSTANCE lib=LoadLibrary(path);
 if (lib!=NULL)
 {
  lua_CFunction f=(lua_CFunction) GetProcAddress(lib,init);
  if (f!=NULL)
  {
   lua_pushcclosure(L,f,0);
   return 1;
  }
 }
 lua_pushnil(L);
 lua_pushstring(L,(lib!=NULL) ? "init" : "open");
 pusherror(L);
 if (lib!=NULL) FreeLibrary(lib);
 return 3;
}

static int register_loadlib(lua_State *L)
{
 lua_register(L,"loadlib",loadlib);
 return 0;
}
