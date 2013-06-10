/*
** $Id: lstate.h,v 1.94 2002/08/12 17:23:12 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"

#include "lobject.h"
#include "ltm.h"


/*
** macros for thread syncronization inside Lua core machine:
** all accesses to the global state and to global objects are syncronized.
** Because threads can read the stack of other threads
** (when running garbage collection),
** a thread must also syncronize any write-access to its own stack.
** Unsyncronized accesses are allowed only when reading its own stack,
** or when reading immutable fields from global objects
** (such as string values and udata values). 
*/
#ifndef lua_lock
#define lua_lock(L)	((void) 0)
#endif

#ifndef lua_unlock
#define lua_unlock(L)	((void) 0)
#endif

/*
** macro to allow the inclusion of user information in Lua state
*/
#ifndef LUA_USERSTATE
#define LUA_USERSTATE
#endif

#ifndef lua_userstateopen
#define lua_userstateopen(l)
#endif



struct lua_longjmp;  /* defined in ldo.c */



/*
** array of `global' objects
*/

#define NUMGLOBS	3

/* default meta table (both for tables and udata) */
#define defaultmeta(L)	(L->globs)

/* table of globals */
#define gt(L)	(L->globs + 1)

/* registry */
#define registry(L)	(L->globs + 2)


/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5


#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)



typedef struct stringtable {
  TString **hash;
  ls_nstr nuse;  /* number of elements */
  int size;
} stringtable;


/*
** informations about a call
*/
typedef struct CallInfo {
  StkId base;  /* base for called function */
  StkId	top;  /* top for this function */
  int state;  /* bit fields; see below */
  union {
    struct {  /* for Lua functions */
      const Instruction *savedpc;
      const Instruction **pc;  /* points to `pc' variable in `luaV_execute' */
      StkId *pb;  /* points to `base' variable in `luaV_execute' */
    } l;
    struct {  /* for C functions */
      int yield_results;
    } c;
  } u;
} CallInfo;


/*
** bit fields for `CallInfo.state'
*/
#define CI_C		1  /* 1 if function is a C function */
/* 1 if (Lua) function has an active `luaV_execute' running it */
#define CI_HASFRAME	2
/* 1 if Lua function is calling another Lua function (and therefore its
   `pc' is being used by the other, and therefore CI_SAVEDPC is 1 too) */
#define CI_CALLING	4
#define CI_SAVEDPC	8  /* 1 if `savedpc' is updated */


#define ci_func(ci)	(clvalue((ci)->base - 1))


/*
** `global state', shared by all threads of this state
*/
typedef struct global_State {
  stringtable strt;  /* hash table for strings */
  Proto *rootproto;  /* list of all prototypes */
  Closure *rootcl;  /* list of all closures */
  Table *roottable;  /* list of all tables */
  UpVal *rootupval;  /* list of closed up values */
  Udata *rootudata;   /* list of all userdata */
  Udata *tmudata;  /* list of userdata to be GC */
  void *Mbuffer;  /* global buffer */
  size_t Mbuffsize;  /* size of Mbuffer */
  lu_mem GCthreshold;
  lu_mem nblocks;  /* number of `bytes' currently allocated */
  lua_CFunction panic;  /* to be called in unprotected errors */
  Node dummynode[1];  /* common node array for all empty tables */
  TString *tmname[TM_N];  /* array with tag-method names */
} global_State;


/*
** `per thread' state
*/
struct lua_State {
  LUA_USERSTATE
  StkId top;  /* first free slot in the stack */
  global_State *l_G;
  CallInfo *ci;  /* call info for current function */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  int stacksize;
  CallInfo *end_ci;  /* points after end of ci array*/
  CallInfo *base_ci;  /* array of CallInfo's */
  int size_ci;  /* size of array `base_ci' */
  unsigned long hookmask;
  ls_count hookcount;
  lua_Hook hook;
  UpVal *openupval;  /* list of open upvalues in this stack */
  struct lua_longjmp *errorJmp;  /* current error recover point */
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
  lua_State *next;  /* circular double linked list of states */
  lua_State *previous;
  TObject globs[NUMGLOBS];  /* registry, table of globals, etc. */
};


#define G(L)	(L->l_G)


void luaE_closethread (lua_State *OL, lua_State *L);

#endif

