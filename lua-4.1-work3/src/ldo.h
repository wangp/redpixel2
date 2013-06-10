/*
** $Id: ldo.h,v 1.36 2001/12/18 20:52:30 roberto Exp $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/

#ifndef ldo_h
#define ldo_h


#include "lobject.h"
#include "lstate.h"


/*
** macro to increment stack top.
** There must be always an empty slot at the L->stack.top
*/
#define incr_top {if (L->top == L->stack_last) luaD_checkstack(L, 1); L->top++;}


#define luaD_checkstack(L,n) if (L->stack_last-(n)<=L->top) luaD_stackerror(L)


void luaD_init (lua_State *L, int stacksize);
void luaD_adjusttop (lua_State *L, StkId newtop);
void luaD_lineHook (lua_State *L, int line, lua_Hook linehook);
void luaD_callHook (lua_State *L, lua_Hook callhook, const char *event);
StkId luaD_precall (lua_State *L, StkId func);
void luaD_call (lua_State *L, StkId func, int nResults);
void luaD_poscall (lua_State *L, int wanted, StkId firstResult);
void luaD_stackerror (lua_State *L);

void luaD_error (lua_State *L, const char *s);
void luaD_breakrun (lua_State *L, int errcode);
int luaD_runprotected (lua_State *L, void (*f)(lua_State *, void *), void *ud);


#endif
