#ifndef __included_mylua_h
#define __included_mylua_h


#include <lua.h>


typedef int lua_ref_t;
typedef int lua_tag_t;

extern lua_State *lua_state;

int mylua_open (int stacksize);
void mylua_close ();

int lua_dofile_path (lua_State *L, const char *filename);
int lua_checkargs (lua_State *L, const char *argtypes);


#endif
