#ifndef __included_scripts_h
#define __included_scripts_h


extern lua_State *lua_state;


void scripts_init ();
void scripts_shutdown ();
void scripts_execute (const char *filenames);


#endif
