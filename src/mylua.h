#ifndef __included_mylua_h
#define __included_mylua_h


#include "lua.h"
#include "lauxlib.h"


typedef int lua_ref_t;

extern lua_State *the_lua_state;
extern lua_State *server_lua_namespace; /* namespace over the_lua_state */
extern lua_State *client_lua_namespace; /* namespace over the_lua_state */

int mylua_open (void);
void mylua_open_server_and_client_namespaces (void);
void mylua_close (void);

/* Like lua_ref, but instead of getting a reference to the top stack
 * item and popping the item, it gets a reference to the item at the
 * index specified.  The stack is left in the original state.
 */
#define lua_toref(L, idx) (lua_pushvalue (L, idx), lua_ref (L, 1))

int lua_call_with_error (lua_State *L, int nargs, int nresults);
int lua_dofile_path (lua_State *L, const char *filename);
int lua_checkargs (lua_State *L, const char *argtypes);


#endif
