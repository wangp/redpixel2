/*
** $Id: luac.h,v 1.23 2001/11/29 01:26:28 lhf Exp $
** definitions for luac
** See Copyright Notice in lua.h
*/

#include "lua.h"

#include "lobject.h"

/* from dump.c */
void luaU_dumpchunk(const Proto* Main, FILE* D);

/* from print.c */
void luaU_printchunk(const Proto* Main);
