#ifndef __included_bindings_h
#define __included_bindings_h


void bindings_init ();
void bindings_shutdown ();


/* Extra functions from sub modules.  */

#include <lua.h>

void set_object_type_register_hook (void (*hook) (const char *, lua_Object,
						  const char *, const char *));


#endif
