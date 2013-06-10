/*
** $Id: ldo.c,v 1.192 2002/08/07 20:55:00 roberto Exp $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/


#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"
#include "lzio.h"



/*
** {======================================================
** Error-recovery functions (based on long jumps)
** =======================================================
*/


/* chain list of long jump buffers */
struct lua_longjmp {
  struct lua_longjmp *previous;
  jmp_buf b;
  volatile int status;  /* error code */
};


static void seterrorobj (lua_State *L, int errcode) {
  switch (errcode) {
    case LUA_ERRMEM: {
      setsvalue(L->top, luaS_new(L, MEMERRMSG));
      break;
    }
    case LUA_ERRERR: {
      setsvalue(L->top, luaS_new(L, "error in error handling"));
      break;
    }
    case LUA_ERRSYNTAX:
    case LUA_ERRRUN: {
      return;  /* error message already on top */
    }
  }
  L->top++;
}


void luaD_throw (lua_State *L, int errcode) {
  if (L->errorJmp) {
    L->errorJmp->status = errcode;
    longjmp(L->errorJmp->b, 1);
  }
  else {
    G(L)->panic(L);
    exit(EXIT_FAILURE);
  }
}


int luaD_rawrunprotected (lua_State *L, Pfunc f, void *ud) {
  struct lua_longjmp lj;
  lj.status = 0;
  lj.previous = L->errorJmp;  /* chain new error handler */
  L->errorJmp = &lj;
  if (setjmp(lj.b) == 0)
    (*f)(L, ud);
  L->errorJmp = lj.previous;  /* restore old error handler */
  return lj.status;
}


static void restore_stack_limit (lua_State *L) {
  L->stack_last = L->stack+L->stacksize-1;
  if (L->size_ci > LUA_MAXCALLS) {  /* there was an overflow? */
    int inuse = (L->ci - L->base_ci);
    if (inuse + 1 < LUA_MAXCALLS)  /* can `undo' overflow? */
      luaD_reallocCI(L, LUA_MAXCALLS);
  }
}

/* }====================================================== */


static void correctstack (lua_State *L, TObject *oldstack) {
  CallInfo *ci;
  UpVal *up;
  L->top = (L->top - oldstack) + L->stack;
  for (up = L->openupval; up != NULL; up = up->next)
    up->v = (up->v - oldstack) + L->stack;
  for (ci = L->base_ci; ci <= L->ci; ci++) {
    ci->base = (ci->base - oldstack) + L->stack;
    ci->top = (ci->top - oldstack) + L->stack;
    if (ci->state & CI_HASFRAME)  /* Lua function with active frame? */
      *ci->u.l.pb = (*ci->u.l.pb - oldstack) + L->stack;  /* correct frame */
  }
}


void luaD_reallocstack (lua_State *L, int newsize) {
  TObject *oldstack = L->stack;
  luaM_reallocvector(L, L->stack, L->stacksize, newsize, TObject);
  L->stacksize = newsize;
  L->stack_last = L->stack+newsize-1-EXTRA_STACK;
  correctstack(L, oldstack);
}


void luaD_reallocCI (lua_State *L, int newsize) {
  CallInfo *oldci = L->base_ci;
  luaM_reallocvector(L, L->base_ci, L->size_ci, newsize, CallInfo);
  L->size_ci = newsize;
  L->ci = (L->ci - oldci) + L->base_ci;
  L->end_ci = L->base_ci + L->size_ci;
}


void luaD_growstack (lua_State *L, int n) {
  if (n <= L->stacksize)  /* double size is enough? */
    luaD_reallocstack(L, 2*L->stacksize);
  else
    luaD_reallocstack(L, L->stacksize + n + EXTRA_STACK);
}


static void luaD_growCI (lua_State *L) {
  if (L->size_ci > LUA_MAXCALLS)  /* overflow while handling overflow? */
    luaD_throw(L, LUA_ERRERR);
  else {
    luaD_reallocCI(L, 2*L->size_ci);
    if (L->size_ci > LUA_MAXCALLS)
      luaG_runerror(L, "stack overflow");
  }
}


void luaD_callhook (lua_State *L, lua_Hookevent event, int line) {
  lua_Hook hook = L->hook;
  if (hook && allowhook(L)) {
    ptrdiff_t top = savestack(L, L->top);
    ptrdiff_t ci_top = savestack(L, L->ci->top);
    lua_Debug ar;
    ar.event = event;
    ar.currentline = line;
    ar.i_ci = L->ci - L->base_ci;
    luaD_checkstack(L, LUA_MINSTACK);  /* ensure minimum stack size */
    L->ci->top = L->top + LUA_MINSTACK;
    setallowhook(L, 0);  /* cannot call hooks inside a hook */
    lua_unlock(L);
    (*hook)(L, &ar);
    lua_lock(L);
    lua_assert(!allowhook(L));
    setallowhook(L, 1);
    L->ci->top = restorestack(L, ci_top);
    L->top = restorestack(L, top);
  }
}


static void adjust_varargs (lua_State *L, int nfixargs, StkId base) {
  int i;
  Table *htab;
  TObject nname;
  int actual = L->top - base;  /* actual number of arguments */
  if (actual < nfixargs) {
    luaD_checkstack(L, nfixargs - actual);
    for (; actual < nfixargs; ++actual)
      setnilvalue(L->top++);
  }
  actual -= nfixargs;  /* number of extra arguments */
  htab = luaH_new(L, 0, 0);  /* create `arg' table */
  for (i=0; i<actual; i++)  /* put extra arguments into `arg' table */
    setobj(luaH_setnum(L, htab, i+1), L->top - actual + i);
  /* store counter in field `n' */
  setsvalue(&nname, luaS_newliteral(L, "n"));
  setnvalue(luaH_set(L, htab, &nname), actual);
  L->top -= actual;  /* remove extra elements from the stack */
  sethvalue(L->top, htab);
  incr_top(L);
}


static StkId tryfuncTM (lua_State *L, StkId func) {
  const TObject *tm = luaT_gettmbyobj(L, func, TM_CALL);
  StkId p;
  ptrdiff_t funcr = savestack(L, func);
  if (!ttisfunction(tm))
    luaG_typeerror(L, func, "call");
  /* Open a hole inside the stack at `func' */
  for (p = L->top; p > func; p--) setobj(p, p-1);
  incr_top(L);
  func = restorestack(L, funcr);  /* previous call may change stack */
  setobj(func, tm);  /* tag method is the new function to be called */
  return func;
}


StkId luaD_precall (lua_State *L, StkId func) {
  LClosure *cl;
  ptrdiff_t funcr = savestack(L, func);
  if (!ttisfunction(func)) /* `func' is not a function? */
    func = tryfuncTM(L, func);  /* check the `function' tag method */
  if (L->ci + 1 == L->end_ci) luaD_growCI(L);
  cl = &clvalue(func)->l;
  if (!cl->isC) {  /* Lua function? prepare its call */
    CallInfo *ci;
    Proto *p = cl->p;
    if (p->is_vararg)  /* varargs? */
      adjust_varargs(L, p->numparams, func+1);
    luaD_checkstack(L, p->maxstacksize);
    ci = ++L->ci;  /* now `enter' new function */
    ci->base = restorestack(L, funcr) + 1;
    ci->top = ci->base + p->maxstacksize;
    ci->u.l.savedpc = p->code;  /* starting point */
    ci->state = CI_SAVEDPC;
    while (L->top < ci->top)
      setnilvalue(L->top++);
    L->top = ci->top;
    return NULL;
  }
  else {  /* if is a C function, call it */
    CallInfo *ci;
    int n;
    luaD_checkstack(L, LUA_MINSTACK);  /* ensure minimum stack size */
    ci = ++L->ci;  /* now `enter' new function */
    ci->base = restorestack(L, funcr) + 1;
    ci->top = L->top + LUA_MINSTACK;
    ci->state = CI_C;  /* a C function */
    if (L->hookmask & LUA_MASKCALL) {
      luaD_callhook(L, LUA_HOOKCALL, -1);
      ci = L->ci;  /* previous call may realocate `ci' */
    }
    lua_unlock(L);
#ifdef LUA_COMPATUPVALUES
    lua_pushupvalues(L);
#endif
    n = (*clvalue(ci->base-1)->c.f)(L);  /* do the actual call */
    lua_lock(L);
    return L->top - n;
  }
}


void luaD_poscall (lua_State *L, int wanted, StkId firstResult) { 
  StkId res;
  if (L->hookmask & LUA_MASKRET) {
    ptrdiff_t fr = savestack(L, firstResult);  /* next call may change stack */
    luaD_callhook(L, LUA_HOOKRET, -1);
    firstResult = restorestack(L, fr);
  }
  res = L->ci->base - 1;  /* res == final position of 1st result */
  L->ci--;
  /* move results to correct place */
  while (wanted != 0 && firstResult < L->top) {
    setobj(res++, firstResult++);
    wanted--;
  }
  while (wanted-- > 0)
    setnilvalue(res++);
  L->top = res;
  luaC_checkGC(L);
}


/*
** Call a function (C or Lua). The function to be called is at *func.
** The arguments are on the stack, right after the function.
** When returns, all the results are on the stack, starting at the original
** function position.
*/ 
void luaD_call (lua_State *L, StkId func, int nResults) {
  StkId firstResult = luaD_precall(L, func);
  if (firstResult == NULL) {  /* is a Lua function? */
    firstResult = luaV_execute(L);  /* call it */
    if (firstResult == NULL) {
      luaD_poscall(L, 0, L->top);
      luaG_runerror(L, "attempt to yield across tag-method/C-call boundary");
    }
  }
  luaD_poscall(L, nResults, firstResult);
}


LUA_API void lua_cobegin (lua_State *L, int nargs) {
  lua_lock(L);
  luaD_precall(L, L->top - (nargs+1));
  lua_unlock(L);
}


static void move_results (lua_State *L, TObject *from, TObject *to) {
  while (from < to) {
    setobj(L->top, from);
    from++;
    incr_top(L);
  }
}


static void resume (lua_State *L, void *ud) {
  StkId firstResult;
  CallInfo *ci = L->ci;
  if (ci->state & CI_C) {  /* not first time (i.e. inside a yield)? */
    /* finish interrupted execution of `OP_CALL' */
    int nresults;
    lua_assert((ci-1)->state & CI_SAVEDPC);
    lua_assert(GET_OPCODE(*((ci-1)->u.l.savedpc - 1)) == OP_CALL ||
               GET_OPCODE(*((ci-1)->u.l.savedpc - 1)) == OP_TAILCALL);
    nresults = GETARG_C(*((ci-1)->u.l.savedpc - 1)) - 1;
    luaD_poscall(L, nresults, L->top);  /* complete it */
    if (nresults >= 0) L->top = L->ci->top;
  }
  firstResult = luaV_execute(L);
  if (firstResult == NULL)   /* yield? */
    *cast(int *, ud) = L->ci->u.c.yield_results;
  else {  /* return */
    *cast(int *, ud) = L->top - firstResult;
    luaD_poscall(L, LUA_MULTRET, firstResult);  /* finalize this coroutine */
  }
}


LUA_API int lua_resume (lua_State *L, lua_State *co) {
  CallInfo *ci;
  int numres;
  int status;
  lua_lock(L);
  ci = co->ci;
  if (ci == co->base_ci)  /* no activation record? ?? */
    luaG_runerror(L, "cannot resume dead thread");
  if (co->errorJmp != NULL)  /* ?? */
    luaG_runerror(L, "cannot resume active thread");
  status = luaD_rawrunprotected(co, resume, &numres);
  if (status == 0)  
    move_results(L, co->top - numres, co->top);
  else {
    setobj(L->top++, co->top - 1);  /* move error message to other stack */
    co->ci = co->base_ci;  /* `kill' thread */
  }
  lua_unlock(L);
  return status;
}


LUA_API int lua_yield (lua_State *L, int nresults) {
  CallInfo *ci;
  lua_lock(L);
  ci = L->ci;
  if ((ci-1)->state & CI_C)
    luaG_runerror(L, "cannot yield a C function");
  lua_assert(ci->state & CI_C);  /* current function is not Lua */
  ci->u.c.yield_results = nresults;
  lua_unlock(L);
  return -1;
}


/*
** Execute a protected call.
*/
struct CallS {  /* data to `f_call' */
  StkId func;
  int nresults;
};


static void f_call (lua_State *L, void *ud) {
  struct CallS *c = cast(struct CallS *, ud);
  luaD_call(L, c->func, c->nresults);
}


int luaD_pcall (lua_State *L, int nargs, int nresults, ptrdiff_t errfunc) {
  struct CallS c;
  int status;
  ptrdiff_t old_top = savestack(L, L->top);
  ptrdiff_t old_ci = saveci(L, L->ci);
  int old_allowhooks = allowhook(L);
  ptrdiff_t old_errfunc = L->errfunc;
  L->errfunc = errfunc;
  c.func = L->top - (nargs+1);  /* function to be called */
  c.nresults = nresults;
  status = luaD_rawrunprotected(L, &f_call, &c);
  if (status != 0) {  /* an error occurred? */
    StkId err;  /* error msg. position */
    seterrorobj(L, status);
    err = L->top - 1;
    /* remove parameters and func from the stack */
    L->top = restorestack(L, old_top) - (nargs+1);
    setobj(L->top++, err);  /* copy error message to corrected top */
    luaF_close(L, L->top);  /* close eventual pending closures */
    L->ci = restoreci(L, old_ci);
    setallowhook(L, old_allowhooks);
    restore_stack_limit(L);
  }
  L->errfunc = old_errfunc;
  return status;
}


/*
** Execute a protected parser.
*/
struct SParser {  /* data to `f_parser' */
  ZIO *z;
  int bin;
};

static void f_parser (lua_State *L, void *ud) {
  struct SParser *p = cast(struct SParser *, ud);
  Proto *tf = p->bin ? luaU_undump(L, p->z) : luaY_parser(L, p->z);
  Closure *cl = luaF_newLclosure(L, 0, gt(L));
  cl->l.p = tf;
  setclvalue(L->top, cl);
  incr_top(L);
}


int luaD_protectedparser (lua_State *L, ZIO *z, int bin) {
  struct SParser p;
  lu_mem old_blocks;
  int status;
  ptrdiff_t oldtopr = savestack(L, L->top);  /* save current top */
  p.z = z; p.bin = bin;
  /* before parsing, give a (good) chance to GC */
  if (G(L)->nblocks + G(L)->nblocks/4 >= G(L)->GCthreshold)
    luaC_collectgarbage(L);
  old_blocks = G(L)->nblocks;
  status = luaD_rawrunprotected(L, f_parser, &p);
  if (status == 0) {
    /* add new memory to threshold (as it probably will stay) */
    lua_assert(G(L)->nblocks >= old_blocks);
    G(L)->GCthreshold += (G(L)->nblocks - old_blocks);
  }
  else {  /* error */
    StkId oldtop = restorestack(L, oldtopr);
    seterrorobj(L, status);
    setobj(oldtop, L->top - 1);  /* copy error message to old top */
    L->top = oldtop+1;
  }
  return status;
}


