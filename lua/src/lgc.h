/*
** $Id: lgc.h,v 1.1 2002/12/28 00:11:38 tjaden Exp $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#ifndef lgc_h
#define lgc_h


#include "lobject.h"


#define luaC_checkGC(L) if (G(L)->nblocks >= G(L)->GCthreshold) \
			  luaC_collectgarbage(L)


void luaC_callallgcTM (lua_State *L);
void luaC_sweep (lua_State *L, int all);
void luaC_collectgarbage (lua_State *L);
void luaC_link (lua_State *L, GCObject *o, lu_byte tt);


#endif
