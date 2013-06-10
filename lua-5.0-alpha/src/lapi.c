/*
** $Id: lapi.c,v 1.211 2002/08/08 20:08:41 roberto Exp $
** Lua API
** See Copyright Notice in lua.h
*/


#include <string.h>

#include "lua.h"

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"


const char lua_ident[] =
  "$Lua: " LUA_VERSION " " LUA_COPYRIGHT " $\n"
  "$Authors: " LUA_AUTHORS " $\n"
  "$URL: www.lua.org $\n";



#ifndef api_check
#define api_check(L, o)		((void)1)
#endif

#define api_checknelems(L, n)	api_check(L, (n) <= (L->top - L->ci->base))

#define api_incr_top(L)   (api_check(L, L->top<L->ci->top), L->top++)




static TObject *negindex (lua_State *L, int index) {
  if (index > LUA_REGISTRYINDEX) {
    api_check(L, index != 0 && -index <= L->top - L->ci->base);
    return L->top+index;
  }
  else switch (index) {  /* pseudo-indices */
    case LUA_REGISTRYINDEX: return registry(L);
    case LUA_GLOBALSINDEX: return gt(L);
    default: {
      TObject *func = (L->ci->base - 1);
      index = LUA_GLOBALSINDEX - index;
      api_check(L, iscfunction(func) && index <= clvalue(func)->c.nupvalues);
      return &clvalue(func)->c.upvalue[index-1];
    }
  }
}


#define luaA_index(L, index) \
  ( (index > 0) ? \
    (api_check(L, index <= L->top - L->ci->base), L->ci->base+index-1) : \
    negindex(L, index))


static TObject *luaA_indexAcceptable (lua_State *L, int index) {
  if (index > 0) {
    TObject *o = L->ci->base+(index-1);
    api_check(L, index <= L->stack_last - L->ci->base);
    if (o >= L->top) return NULL;
    else return o;
  }
  else
    return negindex(L, index);
}


void luaA_pushobject (lua_State *L, const TObject *o) {
  setobj(L->top, o);
  incr_top(L);
}


LUA_API int lua_checkstack (lua_State *L, int size) {
  int res;
  lua_lock(L);
  if ((L->top - L->ci->base + size) > LUA_MAXCSTACK)
    res = 0;  /* stack overflow */
  else {
    luaD_checkstack(L, size);
    if (L->ci->top < L->top + size)
      L->ci->top = L->top + size;
    res = 1;
  }
  lua_unlock(L);
  return res;
}


LUA_API lua_CFunction lua_atpanic (lua_State *L, lua_CFunction panicf) {
  lua_CFunction old;
  lua_lock(L);
  old = G(L)->panic;
  G(L)->panic = panicf;
  lua_unlock(L);
  return old;
}


/*
** basic stack manipulation
*/


LUA_API int lua_gettop (lua_State *L) {
  return (L->top - L->ci->base);
}


LUA_API void lua_settop (lua_State *L, int index) {
  lua_lock(L);
  if (index >= 0) {
    api_check(L, index <= L->stack_last - L->ci->base);
    while (L->top < L->ci->base + index)
      setnilvalue(L->top++);
    L->top = L->ci->base + index;
  }
  else {
    api_check(L, -(index+1) <= (L->top - L->ci->base));
    L->top += index+1;  /* `subtract' index (index is negative) */
  }
  lua_unlock(L);
}


LUA_API void lua_remove (lua_State *L, int index) {
  StkId p;
  lua_lock(L);
  p = luaA_index(L, index);
  while (++p < L->top) setobj(p-1, p);
  L->top--;
  lua_unlock(L);
}


LUA_API void lua_insert (lua_State *L, int index) {
  StkId p;
  StkId q;
  lua_lock(L);
  p = luaA_index(L, index);
  for (q = L->top; q>p; q--) setobj(q, q-1);
  setobj(p, L->top);
  lua_unlock(L);
}


LUA_API void lua_replace (lua_State *L, int index) {
  lua_lock(L);
  api_checknelems(L, 1);
  setobj(luaA_index(L, index), L->top - 1);
  L->top--;
  lua_unlock(L);
}


LUA_API void lua_pushvalue (lua_State *L, int index) {
  lua_lock(L);
  setobj(L->top, luaA_index(L, index));
  api_incr_top(L);
  lua_unlock(L);
}



/*
** access functions (stack -> C)
*/


LUA_API int lua_type (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  return (o == NULL) ? LUA_TNONE : ttype(o);
}


LUA_API const char *lua_typename (lua_State *L, int t) {
  UNUSED(L);
  return (t == LUA_TNONE) ? "no value" : luaT_typenames[t];
}


LUA_API int lua_iscfunction (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  return (o == NULL) ? 0 : iscfunction(o);
}


LUA_API int lua_isnumber (lua_State *L, int index) {
  TObject n;
  const TObject *o = luaA_indexAcceptable(L, index);
  return (o != NULL && tonumber(o, &n));
}


LUA_API int lua_isstring (lua_State *L, int index) {
  int t = lua_type(L, index);
  return (t == LUA_TSTRING || t == LUA_TNUMBER);
}


LUA_API int lua_rawequal (lua_State *L, int index1, int index2) {
  StkId o1 = luaA_indexAcceptable(L, index1);
  StkId o2 = luaA_indexAcceptable(L, index2);
  return (o1 == NULL || o2 == NULL) ? 0  /* index out of range */
                                    : luaO_rawequalObj(o1, o2);
}


LUA_API int lua_equal (lua_State *L, int index1, int index2) {
  StkId o1, o2;
  int i;
  lua_lock(L);  /* may call tag method */
  o1 = luaA_indexAcceptable(L, index1);
  o2 = luaA_indexAcceptable(L, index2);
  i = (o1 == NULL || o2 == NULL) ? 0  /* index out of range */
                                 : equalobj(L, o1, o2);
  lua_unlock(L);
  return i;
}


LUA_API int lua_lessthan (lua_State *L, int index1, int index2) {
  StkId o1, o2;
  int i;
  lua_lock(L);  /* may call tag method */
  o1 = luaA_indexAcceptable(L, index1);
  o2 = luaA_indexAcceptable(L, index2);
  i = (o1 == NULL || o2 == NULL) ? 0  /* index out-of-range */
                                 : luaV_lessthan(L, o1, o2);
  lua_unlock(L);
  return i;
}



LUA_API lua_Number lua_tonumber (lua_State *L, int index) {
  TObject n;
  const TObject *o = luaA_indexAcceptable(L, index);
  if (o != NULL && tonumber(o, &n))
    return nvalue(o);
  else
    return 0;
}


LUA_API int lua_toboolean (lua_State *L, int index) {
  const TObject *o = luaA_indexAcceptable(L, index);
  return (o != NULL) && !l_isfalse(o);
}


LUA_API const char *lua_tostring (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  if (o == NULL)
    return NULL;
  else if (ttisstring(o))
    return svalue(o);
  else {
    const char *s;
    lua_lock(L);  /* `luaV_tostring' may create a new string */
    s = (luaV_tostring(L, o) ? svalue(o) : NULL);
    lua_unlock(L);
    return s;
  }
}


LUA_API size_t lua_strlen (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  if (o == NULL)
    return 0;
  else if (ttisstring(o))
    return tsvalue(o)->tsv.len;
  else {
    size_t l;
    lua_lock(L);  /* `luaV_tostring' may create a new string */
    l = (luaV_tostring(L, o) ? tsvalue(o)->tsv.len : 0);
    lua_unlock(L);
    return l;
  }
}


LUA_API lua_CFunction lua_tocfunction (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  return (o == NULL || !iscfunction(o)) ? NULL : clvalue(o)->c.f;
}


LUA_API void *lua_touserdata (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  if (o == NULL) return NULL;
  switch (ttype(o)) {
    case LUA_TUSERDATA: return (uvalue(o) + 1);
    case LUA_TLIGHTUSERDATA: return pvalue(o);
    default: return NULL;
  }
}


LUA_API const void *lua_topointer (lua_State *L, int index) {
  StkId o = luaA_indexAcceptable(L, index);
  if (o == NULL) return NULL;
  else {
    switch (ttype(o)) {
      case LUA_TTABLE: return hvalue(o);
      case LUA_TFUNCTION: return clvalue(o);
      case LUA_TUSERDATA:
      case LUA_TLIGHTUSERDATA:
        return lua_touserdata(L, index);
      default: return NULL;
    }
  }
}



/*
** push functions (C -> stack)
*/


LUA_API void lua_pushnil (lua_State *L) {
  lua_lock(L);
  setnilvalue(L->top);
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API void lua_pushnumber (lua_State *L, lua_Number n) {
  lua_lock(L);
  setnvalue(L->top, n);
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API void lua_pushlstring (lua_State *L, const char *s, size_t len) {
  lua_lock(L);
  setsvalue(L->top, luaS_newlstr(L, s, len));
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API void lua_pushstring (lua_State *L, const char *s) {
  if (s == NULL)
    lua_pushnil(L);
  else
    lua_pushlstring(L, s, strlen(s));
}


LUA_API const char *lua_pushvfstring (lua_State *L, const char *fmt,
                                      va_list argp) {
  const char *ret;
  lua_lock(L);
  ret = luaO_pushvfstring(L, fmt, argp);
  lua_unlock(L);
  return ret;
}


LUA_API const char *lua_pushfstring (lua_State *L, const char *fmt, ...) {
  const char *ret;
  va_list argp;
  lua_lock(L);
  va_start(argp, fmt);
  ret = luaO_pushvfstring(L, fmt, argp);
  va_end(argp);
  lua_unlock(L);
  return ret;
}


LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n) {
  Closure *cl;
  lua_lock(L);
  api_checknelems(L, n);
  cl = luaF_newCclosure(L, n);
  cl->c.f = fn;
  L->top -= n;
  while (n--)
    setobj(&cl->c.upvalue[n], L->top+n);
  setclvalue(L->top, cl);
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API void lua_pushboolean (lua_State *L, int b) {
  lua_lock(L);
  setbvalue(L->top, (b != 0));  /* ensure that true is 1 */
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API void lua_pushlightuserdata (lua_State *L, void *p) {
  lua_lock(L);
  setpvalue(L->top, p);
  api_incr_top(L);
  lua_unlock(L);
}



/*
** get functions (Lua -> stack)
*/


LUA_API void lua_gettable (lua_State *L, int index) {
  StkId t;
  const TObject *v;
  lua_lock(L);
  t = luaA_index(L, index);
  v = luaV_gettable(L, t, L->top-1, 0);
  setobj(L->top - 1, v);
  lua_unlock(L);
}


LUA_API void lua_rawget (lua_State *L, int index) {
  StkId t;
  lua_lock(L);
  t = luaA_index(L, index);
  api_check(L, ttistable(t));
  setobj(L->top - 1, luaH_get(hvalue(t), L->top - 1));
  lua_unlock(L);
}


LUA_API void lua_rawgeti (lua_State *L, int index, int n) {
  StkId o;
  lua_lock(L);
  o = luaA_index(L, index);
  api_check(L, ttistable(o));
  setobj(L->top, luaH_getnum(hvalue(o), n));
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API void lua_newtable (lua_State *L) {
  lua_lock(L);
  sethvalue(L->top, luaH_new(L, 0, 0));
  api_incr_top(L);
  lua_unlock(L);
}


LUA_API const char *lua_getmode (lua_State *L, int index) {
  static const char *const modes[] = {"", "k", "v", "kv"};
  int mode = 0;
  TObject *t;
  lua_lock(L);
  t = luaA_index(L, index);
  api_check(L, ttistable(t));
  if (hvalue(t)->mode & WEAKKEY) mode += 1;
  if (hvalue(t)->mode & WEAKVALUE) mode += 2;
  lua_unlock(L);
  return modes[mode];
}


LUA_API int lua_getmetatable (lua_State *L, int objindex) {
  StkId obj;
  Table *mt;
  int res;
  lua_lock(L);
  obj = luaA_indexAcceptable(L, objindex);
  switch (ttype(obj)) {
    case LUA_TTABLE:
      mt = hvalue(obj)->metatable;
      break;
    case LUA_TUSERDATA:
      mt = uvalue(obj)->uv.metatable;
      break;
    default:
      mt = hvalue(defaultmeta(L));
  }
  if (mt == hvalue(defaultmeta(L)))
    res = 0;
  else {
    sethvalue(L->top, mt);
    api_incr_top(L);
    res = 1;
  }
  lua_unlock(L);
  return res;
}


LUA_API void lua_getglobals (lua_State *L, int index) {
  StkId o;
  lua_lock(L);
  o = luaA_index(L, index);
  setobj(L->top, isLfunction(o) ? &clvalue(o)->l.g : gt(L));
  api_incr_top(L);
  lua_unlock(L);
}


/*
** set functions (stack -> Lua)
*/


LUA_API void lua_settable (lua_State *L, int index) {
  StkId t;
  lua_lock(L);
  api_checknelems(L, 2);
  t = luaA_index(L, index);
  luaV_settable(L, t, L->top - 2, L->top - 1);
  L->top -= 2;  /* pop index and value */
  lua_unlock(L);
}


LUA_API void lua_rawset (lua_State *L, int index) {
  StkId t;
  lua_lock(L);
  api_checknelems(L, 2);
  t = luaA_index(L, index);
  api_check(L, ttistable(t));
  setobj(luaH_set(L, hvalue(t), L->top-2), L->top-1);
  L->top -= 2;
  lua_unlock(L);
}


LUA_API void lua_rawseti (lua_State *L, int index, int n) {
  StkId o;
  lua_lock(L);
  api_checknelems(L, 1);
  o = luaA_index(L, index);
  api_check(L, ttistable(o));
  setobj(luaH_setnum(L, hvalue(o), n), L->top-1);
  L->top--;
  lua_unlock(L);
}


LUA_API void lua_setmode (lua_State *L, int index, const char *mode) {
  TObject *t;
  lua_lock(L);
  t = luaA_index(L, index);
  api_check(L, ttistable(t));
  hvalue(t)->mode &= ~(WEAKKEY | WEAKVALUE);  /* clear bits */
  if (strchr(mode, 'k')) hvalue(t)->mode |= WEAKKEY;
  if (strchr(mode, 'v')) hvalue(t)->mode |= WEAKVALUE;
  lua_unlock(L);
}

LUA_API int lua_setmetatable (lua_State *L, int objindex) {
  TObject *obj, *mt;
  int res = 1;
  lua_lock(L);
  api_checknelems(L, 1);
  obj = luaA_index(L, objindex);
  mt = (!ttisnil(L->top - 1)) ? L->top - 1 : defaultmeta(L);
  api_check(L, ttistable(mt));
  switch (ttype(obj)) {
    case LUA_TTABLE: {
      hvalue(obj)->metatable = hvalue(mt);
      break;
    }
    case LUA_TUSERDATA: {
      uvalue(obj)->uv.metatable = hvalue(mt);
      break;
    }
    default: {
      res = 0;  /* cannot set */
      break;
    }
  }
  L->top--;
  lua_unlock(L);
  return res;
}


LUA_API int lua_setglobals (lua_State *L, int index) {
  StkId o;
  int res = 0;
  lua_lock(L);
  api_checknelems(L, 1);
  o = luaA_index(L, index);
  L->top--;
  api_check(L, ttistable(L->top));
  if (isLfunction(o)) {
    res = 1;
    clvalue(o)->l.g = *(L->top);
  }
  lua_unlock(L);
  return res;
}


/*
** `load' and `call' functions (run Lua code)
*/

LUA_API void lua_call (lua_State *L, int nargs, int nresults) {
  StkId func;
  lua_lock(L);
  api_checknelems(L, nargs+1);
  func = L->top - (nargs+1);
  luaD_call(L, func, nresults);
  lua_unlock(L);
}


LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc) {
  int status;
  ptrdiff_t func;
  lua_lock(L);
  func = (errfunc == 0) ? 0 : savestack(L, luaA_index(L, errfunc));
  status = luaD_pcall(L, nargs, nresults, func);
  lua_unlock(L);
  return status;
}


LUA_API int lua_load (lua_State *L, lua_Chunkreader reader, void *data,
                      const char *chunkname) {
  ZIO z;
  int status;
  int c;
  lua_lock(L);
  if (!chunkname) chunkname = "?";
  luaZ_init(&z, reader, data, chunkname);
  c = luaZ_lookahead(&z);
  status = luaD_protectedparser(L, &z, (c == LUA_SIGNATURE[0]));
  lua_unlock(L);
  return status;
}


/*
** Garbage-collection functions
*/

/* GC values are expressed in Kbytes: #bytes/2^10 */
#define GCscale(x)		(cast(int, (x)>>10))
#define GCunscale(x)		(cast(lu_mem, (x)<<10))

LUA_API int lua_getgcthreshold (lua_State *L) {
  int threshold;
  lua_lock(L);
  threshold = GCscale(G(L)->GCthreshold);
  lua_unlock(L);
  return threshold;
}

LUA_API int lua_getgccount (lua_State *L) {
  int count;
  lua_lock(L);
  count = GCscale(G(L)->nblocks);
  lua_unlock(L);
  return count;
}

LUA_API void lua_setgcthreshold (lua_State *L, int newthreshold) {
  lua_lock(L);
  if (newthreshold > GCscale(ULONG_MAX))
    G(L)->GCthreshold = ULONG_MAX;
  else
    G(L)->GCthreshold = GCunscale(newthreshold);
  luaC_checkGC(L);
  lua_unlock(L);
}


/*
** miscellaneous functions
*/


LUA_API int lua_error (lua_State *L) {
  lua_lock(L);
  api_checknelems(L, 1);
  luaG_errormsg(L);
  lua_unlock(L);
  return 0;  /* to avoid warnings */
}


LUA_API int lua_next (lua_State *L, int index) {
  StkId t;
  int more;
  lua_lock(L);
  t = luaA_index(L, index);
  api_check(L, ttistable(t));
  more = luaH_next(L, hvalue(t), L->top - 1);
  if (more) {
    api_incr_top(L);
  }
  else  /* no more elements */
    L->top -= 1;  /* remove key */
  lua_unlock(L);
  return more;
}


LUA_API void lua_concat (lua_State *L, int n) {
  lua_lock(L);
  api_checknelems(L, n);
  if (n >= 2) {
    luaV_concat(L, n, L->top - L->ci->base - 1);
    L->top -= (n-1);
    luaC_checkGC(L);
  }
  else if (n == 0) {  /* push empty string */
    setsvalue(L->top, luaS_newlstr(L, NULL, 0));
    api_incr_top(L);
  }
  /* else n == 1; nothing to do */
  lua_unlock(L);
}


LUA_API void *lua_newuserdata (lua_State *L, size_t size) {
  Udata *u;
  lua_lock(L);
  u = luaS_newudata(L, size);
  setuvalue(L->top, u);
  api_incr_top(L);
  lua_unlock(L);
  return u + 1;
}


LUA_API int lua_pushupvalues (lua_State *L) {
  Closure *func;
  int n, i;
  lua_lock(L);
  api_check(L, iscfunction(L->ci->base - 1));
  func = clvalue(L->ci->base - 1);
  n = func->c.nupvalues;
  luaD_checkstack(L, n + LUA_MINSTACK);
  for (i=0; i<n; i++) {
    setobj(L->top, &func->c.upvalue[i]);
    L->top++;
  }
  lua_unlock(L);
  return n;
}


