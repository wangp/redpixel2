/*
* min.c
* a minimal Lua interpreter. loads stdin only.
* no standard library, only a "print" function and a simple error handler.
*/

#include <stdio.h>
#include "lua.h"

/* a simple "print". based on the code in lbaselib.c */
static int print(lua_State *L)
{
 int n=lua_gettop(L);
 int i;
 for (i=1; i<=n; i++)
 {
  if (i>1) printf("\t");
  if (lua_isstring(L,i))
   printf("%s",lua_tostring(L,i));
  else
   printf("%s:%p",lua_type(L,i),lua_topointer(L,i));
 }
 printf("\n");
 return 0;
}

/* a simple error handler (to be registered as _ERRORMESSAGE) */
static int error(lua_State *L) 
{
 fprintf(stderr,"error: %s\n",lua_tostring(L,1));
 return 0;
}

int main(void)
{
 lua_State *L=lua_open(0);
 lua_register(L,"print",print);
 lua_register(L,LUA_ERRORMESSAGE,error);
 return lua_dofile(L,NULL);
}
