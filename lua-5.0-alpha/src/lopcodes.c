/*
** $Id: lopcodes.c,v 1.20 2002/06/12 14:51:31 roberto Exp $
** extracted automatically from lopcodes.h by mkprint.lua
** DO NOT EDIT
** See Copyright Notice in lua.h
*/


#include "lua.h"

#include "lobject.h"
#include "lopcodes.h"


#ifdef LUA_OPNAMES

const char *const luaP_opnames[] = {
  "MOVE",
  "LOADK",
  "LOADBOOL",
  "LOADNIL",
  "GETUPVAL",
  "GETGLOBAL",
  "GETTABLE",
  "SETGLOBAL",
  "SETUPVAL",
  "SETTABLE",
  "NEWTABLE",
  "SELF",
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "POW",
  "UNM",
  "NOT",
  "CONCAT",
  "JMP",
  "EQ",
  "LT",
  "LE",
  "GT",
  "GE",
  "TEST",
  "CALL",
  "TAILCALL",
  "RETURN",
  "FORLOOP",
  "TFORLOOP",
  "TFORPREP",
  "SETLIST",
  "SETLISTO",
  "CLOSE",
  "CLOSURE"
};

#endif

#define opmode(t,x,b,c,sa,k,m) (((t)<<OpModeT) | \
   ((b)<<OpModeBreg) | ((c)<<OpModeCreg) | \
   ((sa)<<OpModesetA) | ((k)<<OpModeK) | (m))


const lu_byte luaP_opmodes[NUM_OPCODES] = {
/*       T _ B C sA K mode		   opcode    */
  opmode(0,0,1,0, 1,0,iABC)		/* OP_MOVE */
 ,opmode(0,0,0,0, 1,1,iABx)		/* OP_LOADK */
 ,opmode(0,0,0,0, 1,0,iABC)		/* OP_LOADBOOL */
 ,opmode(0,0,1,0, 1,0,iABC)		/* OP_LOADNIL */
 ,opmode(0,0,0,0, 1,0,iABC)		/* OP_GETUPVAL */
 ,opmode(0,0,0,0, 1,1,iABx)		/* OP_GETGLOBAL */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_GETTABLE */
 ,opmode(0,0,0,0, 0,1,iABx)		/* OP_SETGLOBAL */
 ,opmode(0,0,0,0, 0,0,iABC)		/* OP_SETUPVAL */
 ,opmode(0,0,1,1, 0,0,iABC)		/* OP_SETTABLE */
 ,opmode(0,0,0,0, 1,0,iABC)		/* OP_NEWTABLE */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_SELF */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_ADD */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_SUB */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_MUL */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_DIV */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_POW */
 ,opmode(0,0,1,0, 1,0,iABC)		/* OP_UNM */
 ,opmode(0,0,1,0, 1,0,iABC)		/* OP_NOT */
 ,opmode(0,0,1,1, 1,0,iABC)		/* OP_CONCAT */
 ,opmode(0,0,0,0, 0,0,iAsBx)		/* OP_JMP */
 ,opmode(1,0,0,1, 0,0,iABC)		/* OP_EQ */
 ,opmode(1,0,0,1, 0,0,iABC)		/* OP_LT */
 ,opmode(1,0,0,1, 0,0,iABC)		/* OP_LE */
 ,opmode(1,0,0,1, 0,0,iABC)		/* OP_GT */
 ,opmode(1,0,0,1, 0,0,iABC)		/* OP_GE */
 ,opmode(1,0,0,1, 1,0,iABC)		/* OP_TEST */
 ,opmode(0,0,0,0, 0,0,iABC)		/* OP_CALL */
 ,opmode(0,0,0,0, 0,0,iABC)		/* OP_TAILCALL */
 ,opmode(0,0,0,0, 0,0,iABC)		/* OP_RETURN */
 ,opmode(0,0,0,0, 0,0,iAsBx)		/* OP_FORLOOP */
 ,opmode(1,0,0,0, 0,0,iABC)		/* OP_TFORLOOP */
 ,opmode(0,0,0,0, 0,0,iAsBx)		/* OP_TFORPREP */
 ,opmode(0,0,0,0, 0,0,iABx)		/* OP_SETLIST */
 ,opmode(0,0,0,0, 0,0,iABx)		/* OP_SETLISTO */
 ,opmode(0,0,0,0, 0,0,iABC)		/* OP_CLOSE */
 ,opmode(0,0,0,0, 1,0,iABx)		/* OP_CLOSURE */
};

