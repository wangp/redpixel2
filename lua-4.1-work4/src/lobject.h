/*
** $Id: lobject.h,v 1.124 2002/02/08 22:42:41 roberto Exp $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/

#ifndef lobject_h
#define lobject_h


#include "llimits.h"
#include "lua.h"


#ifndef lua_assert
#define lua_assert(c)		/* empty */
#endif


#ifndef UNUSED
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
#endif


#ifndef cast
#define cast(t, exp)	((t)(exp))
#endif


/* tags for values visible from Lua */
#define NUM_TAGS	6


typedef union {
  union TString *ts;
  union Udata *u;
  union Closure *cl;
  struct Table *h;
  struct lua_TObject *v;
  lua_Number n;
  int b;
} Value;


typedef struct lua_TObject {
  int tt;
  Value value;
} TObject;


/* Macros to access values */
#define ttype(o)        ((o)->tt)
#define nvalue(o)       ((o)->value.n)
#define tsvalue(o)      ((o)->value.ts)
#define uvalue(o)      ((o)->value.u)
#define clvalue(o)      ((o)->value.cl)
#define hvalue(o)       ((o)->value.h)
#define vvalue(o)	((o)->value.v)
#define bvalue(o)	((o)->value.b)

#define l_isfalse(o)	(ttype(o) == LUA_TNIL || \
			(ttype(o) == LUA_TBOOLEAN && bvalue(o) == 0))

/* Macros to set values */
#define setnvalue(obj,x) \
  { TObject *_o=(obj); _o->tt=LUA_TNUMBER; _o->value.n=(x); }

#define chgnvalue(obj,x)	((obj)->value.n=(x))

#define setbvalue(obj,x) \
  { TObject *_o=(obj); _o->tt=LUA_TBOOLEAN; _o->value.b=(x); }

#define setsvalue(obj,x) \
  { TObject *_o=(obj); _o->tt=LUA_TSTRING; _o->value.ts=(x); }

#define setuvalue(obj,x) \
  { TObject *_o=(obj); _o->tt=LUA_TUSERDATA; _o->value.u=(x); }

#define setclvalue(obj,x) \
  { TObject *_o=(obj); _o->tt=LUA_TFUNCTION; _o->value.cl=(x); }

#define sethvalue(obj,x) \
  { TObject *_o=(obj); _o->tt=LUA_TTABLE; _o->value.h=(x); }

#define setnilvalue(obj) ((obj)->tt=LUA_TNIL)

#define setupvalue(obj,x,t) \
  { TObject *_o=(obj); _o->tt=(t); _o->value.v=(x); }

#define setobj(obj1,obj2) \
  { TObject *o1=(obj1); const TObject *o2=(obj2); \
    o1->tt=o2->tt; o1->value = o2->value; }

#define setttype(obj, tt) (ttype(obj) = (tt))



typedef TObject *StkId;  /* index to stack elements */


/*
** String headers for string table
*/
typedef union TString {
  union L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  struct {
    lu_hash hash;
    size_t len;
    int marked;
    union TString *nexthash;  /* chain for hash table */
  } tsv;
} TString;


#define getstr(ts)	cast(const char *, (ts) + 1)
#define svalue(o)       getstr(tsvalue(o))



typedef union Udata {
  union L_Umaxalign dummy;  /* ensures maximum alignment for `local' udata */
  struct {
    struct Table *metatable;
    void *value;
    union Udata *next;  /* chain for list of all udata */
    size_t len;  /* least bit reserved for gc mark */
  } uv;
} Udata;




/*
** Function Prototypes
*/
typedef struct Proto {
  TObject *k;  /* constants used by the function */
  Instruction *code;
  struct Proto **p;  /* functions defined inside the function */
  struct Proto *next;
  int *lineinfo;  /* map from opcodes to source lines */
  struct LocVar *locvars;  /* information about local variables */
  TString  *source;
  int sizek;  /* size of `k' */
  int sizecode;
  int sizep;  /* size of `p' */
  int sizelineinfo;  /* size of `lineinfo' */
  int sizelocvars;
  int lineDefined;
  short nupvalues;
  short numparams;
  short is_vararg;
  short maxstacksize;
  short marked;
} Proto;


typedef struct LocVar {
  TString *varname;
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
} LocVar;



/*
** Upvalues
*/

typedef struct UpVal {
  TObject *v;  /* points to stack or to its own value */
  struct UpVal *next;
  TObject value;  /* the value (when closed) */
} UpVal;


/*
** Closures
*/

typedef struct CClosure {
  lu_byte isC;  /* 0 for Lua functions, 1 for C functions */
  lu_byte nupvalues;
  lu_byte marked;
  union Closure *next;
  lua_CFunction f;
  TObject upvalue[1];
} CClosure;


typedef struct LClosure {
  lu_byte isC;
  lu_byte nupvalues;
  lu_byte marked;
  union Closure *next;  /* first four fields must be equal to CClosure!! */
  struct Proto *p;
  UpVal *upvals[1];
} LClosure;


typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;


#define iscfunction(o)	(ttype(o) == LUA_TFUNCTION && clvalue(o)->c.isC)



/*
** Tables
*/

typedef struct Node {
  TObject _key;
  TObject _val;
  struct Node *next;  /* for chaining */
} Node;


typedef struct Table {
  struct Table *metatable;
  TObject *array;  /* array part */
  Node *node;
  Node *firstfree;  /* this position is free; all positions after it are full */
  struct Table *next;
  struct Table *mark;  /* marked tables (point to itself when not marked) */
  int sizearray;  /* size of `array' array */
  unsigned short flags;  /* 1<<p means tagmethod(p) is not present */ 
  lu_byte lsizenode;  /* log2 of size of `node' array */
} Table;


/* unmarked tables are represented by pointing `mark' to themselves */
#define ismarked(x)	((x)->mark != (x))


/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size)	(cast(int, (s) & ((size)-1)))


#define twoto(x)	(1<<(x))
#define sizenode(t)	(twoto((t)->lsizenode))
#define sizearray(t)	((t)->sizearray)


extern const TObject luaO_nilobject;

int luaO_log2 (unsigned int x);


#define luaO_openspace(L,n,t)	cast(t *, luaO_openspaceaux(L,(n)*sizeof(t)))
void *luaO_openspaceaux (lua_State *L, size_t n);

int luaO_equalObj (const TObject *t1, const TObject *t2);
int luaO_str2d (const char *s, lua_Number *result);

void luaO_verror (lua_State *L, const char *fmt, ...);
void luaO_chunkid (char *out, const char *source, int len);


#endif
