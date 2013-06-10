/*
** $Id: dump.c,v 1.37 2002/08/07 00:36:03 lhf Exp $
** save bytecodes
** See Copyright Notice in lua.h
*/

#include <stddef.h>

#include "lobject.h"
#include "lopcodes.h"
#include "lundump.h"

#define DumpBlock(b,size,D)	(*D->write)(b,size,D->data)
#define DumpVector(b,n,size,D)	DumpBlock(b,(n)*(size),D)
#define DumpLiteral(s,D)	DumpBlock("" s,(sizeof(s))-1,D)

typedef struct {
 Writer write;
 void *data;
} DumpState;

static void DumpByte(int y, DumpState* D)
{
 char x=(char)y;
 DumpBlock(&x,sizeof(x),D);
}

static void DumpInt(int x, DumpState* D)
{
 DumpBlock(&x,sizeof(x),D);
}

static void DumpSize(size_t x, DumpState* D)
{
 DumpBlock(&x,sizeof(x),D);
}

static void DumpNumber(lua_Number x, DumpState* D)
{
 DumpBlock(&x,sizeof(x),D);
}

static void DumpString(TString* s, DumpState* D)
{
 if (s==NULL || getstr(s)==NULL)
  DumpSize(0,D);
 else
 {
  size_t size=s->tsv.len+1;		/* include trailing '\0' */
  DumpSize(size,D);
  DumpBlock(getstr(s),size,D);
 }
}

static void DumpCode(const Proto* f, DumpState* D)
{
 DumpInt(f->sizecode,D);
 DumpVector(f->code,f->sizecode,sizeof(*f->code),D);
}

static void DumpLocals(const Proto* f, DumpState* D)
{
 int i,n=f->sizelocvars;
 DumpInt(n,D);
 for (i=0; i<n; i++)
 {
  DumpString(f->locvars[i].varname,D);
  DumpInt(f->locvars[i].startpc,D);
  DumpInt(f->locvars[i].endpc,D);
 }
}

static void DumpLines(const Proto* f, DumpState* D)
{
 if (f->lineinfo==NULL)
  DumpInt(0,D);
 else
 {
  DumpInt(f->sizecode,D);
  DumpVector(f->lineinfo,f->sizecode,sizeof(*f->lineinfo),D);
 }
}

static void DumpFunction(const Proto* f, const TString* p, DumpState* D);

static void DumpConstants(const Proto* f, DumpState* D)
{
 int i,n;
 DumpInt(n=f->sizek,D);
 for (i=0; i<n; i++)
 {
  const TObject* o=&f->k[i];
  DumpByte(ttype(o),D);
  switch (ttype(o))
  {
   case LUA_TNUMBER:
	DumpNumber(nvalue(o),D);
	break;
   case LUA_TSTRING:
	DumpString(tsvalue(o),D);
	break;
   case LUA_TNIL:
	break;
   default:
	lua_assert(0);			/* cannot happen */
	break;
  }
 }
 DumpInt(n=f->sizep,D);
 for (i=0; i<n; i++) DumpFunction(f->p[i],f->source,D);
}

static void DumpFunction(const Proto* f, const TString* p, DumpState* D)
{
 DumpString((f->source==p) ? NULL : f->source,D);
 DumpInt(f->lineDefined,D);
 DumpByte(f->nupvalues,D);
 DumpByte(f->numparams,D);
 DumpByte(f->is_vararg,D);
 DumpByte(f->maxstacksize,D);
 DumpLocals(f,D);
 DumpLines(f,D);
 DumpConstants(f,D);
 DumpCode(f,D);
}

static void DumpHeader(DumpState* D)
{
 DumpLiteral(LUA_SIGNATURE,D);
 DumpByte(VERSION,D);
 DumpByte(luaU_endianness(),D);
 DumpByte(sizeof(int),D);
 DumpByte(sizeof(size_t),D);
 DumpByte(sizeof(Instruction),D);
 DumpByte(SIZE_OP,D);
 DumpByte(SIZE_A,D);
 DumpByte(SIZE_B,D);
 DumpByte(SIZE_C,D);
 DumpByte(sizeof(lua_Number),D);
 DumpNumber(TEST_NUMBER,D);
}

void luaU_dump(const Proto* Main, Writer w, void* data)
{
 DumpState D;
 D.write=w;
 D.data=data;
 DumpHeader(&D);
 DumpFunction(Main,NULL,&D);
}
