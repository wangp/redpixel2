#ifndef __included_luastack_h
#define __included_luastack_h


void lua_enablestacktraceback (lua_State *);
void lua_set_alert (lua_State *L, int (*alertfunc)(lua_State *));


#endif