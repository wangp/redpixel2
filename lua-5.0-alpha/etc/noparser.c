/*
* The code below can be used to make a Lua core that does not contain the
* parsing modules (lcode, llex, lparser), which represent 37% of the total core.
* You'll only be able to load binary files and strings, precompiled with luac.
* (Of course, you'll have to build luac with the original parsing modules!)
*/

#include "llex.h"
#include "lparser.h"
#include "lzio.h"

void luaX_init (lua_State *L) {
  UNUSED(L);
}

Proto *luaY_parser (lua_State *L, ZIO *z) {
  UNUSED(z);
  lua_pushstring(L,"parser not loaded");
  lua_error(L);
  return NULL;
}