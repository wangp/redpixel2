/*
 * This was extracted from `liolib.c' in the Lua distribution.
 * Modified slightly to use Allegro's Unicode routines (may or may not help).
 */


#include <allegro.h>

#define LUA_REENTRANT
#include <lua.h>
#include <luadebug.h>
#include <lauxlib.h>


#define MESSAGESIZE	150
#define MAXMESSAGE (MESSAGESIZE*10)

static void errorfb (lua_State *L) {
  char buff[MAXMESSAGE];
  int level = 1;  /* skip level 0 (it's this function) */
  lua_Debug ar;
  lua_Object alertfunc = lua_rawgetglobal(L, "_ALERT");
  usprintf(buff, "error: %.200s\n", lua_getstring(L, lua_getparam(L, 1)));
  while (lua_getstack(L, level++, &ar)) {
    char buffchunk[60];
    lua_getinfo(L, "Snl", &ar);
    luaL_chunkid(buffchunk, ar.source, sizeof(buffchunk));
    if (level == 2) ustrcat(buff, "stack traceback:\n");
    ustrcat(buff, "  ");
    if (strlen(buff) > MAXMESSAGE-MESSAGESIZE) {
      ustrcat(buff, "...\n");
      break;  /* buffer is full */
    }
    switch (*ar.namewhat) {
      case 'g':  case 'l':  /* global, local */
        usprintf(buff+ustrlen(buff), "function `%.50s'", ar.name);
        break;
      case 'f':  /* field */
        usprintf(buff+ustrlen(buff), "method `%.50s'", ar.name);
        break;
      case 't':  /* tag method */
        usprintf(buff+ustrlen(buff), "`%.50s' tag method", ar.name);
        break;
      default: {
        if (*ar.what == 'm')  /* main? */
          usprintf(buff+ustrlen(buff), "main of %.70s", buffchunk);
        else if (*ar.what == 'C')  /* C function? */
          usprintf(buff+ustrlen(buff), "%.70s", buffchunk);
        else
          usprintf(buff+ustrlen(buff), "function <%d:%.70s>",
                  ar.linedefined, buffchunk);
        ar.source = NULL;
      }
    }
    if (ar.currentline > 0)
      usprintf(buff+ustrlen(buff), " at line %d", ar.currentline);
    if (ar.source)
      usprintf(buff+ustrlen(buff), " [%.70s]", buffchunk);
    ustrcat(buff, "\n");
  }
  if (lua_isfunction(L, alertfunc)) {  /* avoid loop if _ALERT is not defined */
    lua_pushstring(L, buff);
    lua_callfunction(L, alertfunc);
  }
}


void lua_enablestacktraceback (lua_State *L)
{
  lua_register (L, "_ERRORMESSAGE", errorfb);
}


void lua_set_alert (lua_State *L, void (*alertfunc)(lua_State *L))
{
  lua_register (L, "_ALERT", alertfunc);
}
