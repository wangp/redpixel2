/*
 * This was extracted from `liolib.c' in the Lua distribution.
 */


#include <allegro.h>

#include <lua.h>
#include <luadebug.h>
#include <lauxlib.h>


#define LEVELS1	12	/* size of the first part of the stack */
#define LEVELS2	10	/* size of the second part of the stack */

static int errorfb (lua_State *L) {
  int level = 1;  /* skip level 0 (it's this function) */
  int firstpart = 1;  /* still before eventual `...' */
  lua_Debug ar;
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  luaL_addstring(&b, "error: ");
  luaL_addstring(&b, luaL_check_string(L, 1));
  luaL_addstring(&b, "\n");
  while (lua_getstack(L, level++, &ar)) {
    char buff[120];  /* enough to fit following `sprintf's */
    if (level == 2)
      luaL_addstring(&b, "stack traceback:\n");
    else if (level > LEVELS1 && firstpart) {
      /* no more than `LEVELS2' more levels? */
      if (!lua_getstack(L, level+LEVELS2, &ar))
        level--;  /* keep going */
      else {
        luaL_addstring(&b, "       ...\n");  /* too many levels */
        while (lua_getstack(L, level+LEVELS2, &ar))  /* find last levels */
          level++;
      }
      firstpart = 0;
      continue;
    }
    sprintf(buff, "%4d:  ", level-1);
    luaL_addstring(&b, buff);
    lua_getinfo(L, "Snl", &ar);
    switch (*ar.namewhat) {
      case 'g':  case 'l':  /* global, local */
        sprintf(buff, "function `%.50s'", ar.name);
        break;
      case 'f':  /* field */
        sprintf(buff, "method `%.50s'", ar.name);
        break;
      case 't':  /* tag method */
        sprintf(buff, "`%.50s' tag method", ar.name);
        break;
      default: {
        if (*ar.what == 'm')  /* main? */
          sprintf(buff, "main of %.70s", ar.short_src);
        else if (*ar.what == 'C')  /* C function? */
          sprintf(buff, "%.70s", ar.short_src);
        else
          sprintf(buff, "function <%d:%.70s>", ar.linedefined, ar.short_src);
        ar.source = NULL;  /* do not print source again */
      }
    }
    luaL_addstring(&b, buff);
    if (ar.currentline > 0) {
      sprintf(buff, " at line %d", ar.currentline);
      luaL_addstring(&b, buff);
    }
    if (ar.source) {
      sprintf(buff, " [%.70s]", ar.short_src);
      luaL_addstring(&b, buff);
    }
    luaL_addstring(&b, "\n");
  }
  luaL_pushresult(&b);
  lua_getglobal(L, "_ALERT");
  if (lua_isfunction(L, -1)) {  /* avoid loop if _ALERT is not defined */
    lua_pushvalue(L, -2);  /* error message */
    lua_rawcall(L, 1, 0);
  }
  return 0;
}


void lua_enablestacktraceback (lua_State *L)
{
  lua_register (L, "_ERRORMESSAGE", errorfb);
}


void lua_set_alert (lua_State *L, int (*alertfunc)(lua_State *))
{
  lua_register (L, "_ALERT", alertfunc);
}
