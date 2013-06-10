/*
** $Id: ltm.h,v 1.31 2002/01/09 21:50:35 roberto Exp $
** Tag methods
** See Copyright Notice in lua.h
*/

#ifndef ltm_h
#define ltm_h


#include "lobject.h"

/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM"
*/
typedef enum {
  TM_GETTABLE = 0,
  TM_SETTABLE,
  TM_INDEX,
  TM_GC,
  TM_WEAKMODE,
  TM_ADD,
  TM_SUB,
  TM_MUL,
  TM_DIV,
  TM_POW,
  TM_UNM,
  TM_LT,
  TM_CONCAT,
  TM_CALL,
  TM_N		/* number of elements in the enum */
} TMS;



#define fasttm(l,et,e) \
  (((et)->flags & (1<<(e))) ? NULL : luaT_gettm(et, e, G(l)->tmname[e]))


const TObject *luaT_gettm (Table *events, TMS event, TString *ename);
const TObject *luaT_gettmbyobj (lua_State *L, const TObject *o, TMS event);
void luaT_init (lua_State *L);

extern const char *const luaT_typenames[];

#endif
