/*
** $Id: ltm.c,v 1.84 2001/12/11 22:48:44 roberto Exp $
** Tag methods
** See Copyright Notice in lua.h
*/


#include <stdio.h>
#include <string.h>

#include "lua.h"

#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"



const char *const luaT_typenames[] = {
  "userdata", "nil", "number", "boolean", "string", "table", "function"
};


void luaT_init (lua_State *L) {
  static const char *const luaT_eventname[] = {  /* ORDER TM */
    "gettable", "settable", "index",
    "gc",
    "add", "sub", "mul", "div",
    "pow", "unm", "lt", "concat",
    "call"
  };
  int i;
  for (i=0; i<TM_N; i++) {
    G(L)->tmname[i] = luaS_new(L, luaT_eventname[i]);
    G(L)->tmname[i]->tsv.marked = FIXMARK;  /* never collect these names */
  }
}


/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
const TObject *luaT_gettm (Table *events, TMS event, TString *ename) {
  const TObject *tm = luaH_getstr(events, ename);
  if (ttype(tm) == LUA_TNIL) {  /* no tag method? */
    events->flags |= (1<<event);  /* cache this fact */
    return NULL;
  }
  else return tm;
}


const TObject *luaT_gettmbyobj (lua_State *L, const TObject *o, TMS event) {
  TString *ename = G(L)->tmname[event];
  switch (ttype(o)) {
    case LUA_TTABLE:
      return luaH_getstr(hvalue(o)->eventtable, ename);
    case LUA_TUSERDATA:
      return luaH_getstr(uvalue(o)->uv.eventtable, ename);
    default:
      return &luaO_nilobject;
  }
}

