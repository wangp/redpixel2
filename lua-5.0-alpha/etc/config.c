/* config.c -- configuration for Lua interpreter
*
* #define LUA_USERCONFIG to this file
*
* #define USE_LOADLIB to use bare-bones dynamic loader
* #define USE_READLINE to use GNU readline for line editing and history
* #define lua_userinit(L) to a Lua function that loads libraries; typically
*  #define lua_userinit(L)	openstdlibs(L)+myinit(L)
*  #define lua_userinit(L)	myinit(L)
* loadlib.c defines lua_userinit in the first form
*
* The only other things that can be customized in lua.c are PROMPT and PROMPT2 
*/

#ifdef USE_LOADLIB
#include "loadlib.c"
#endif

#ifdef USE_READLINE
#include "readline.c"
#endif

