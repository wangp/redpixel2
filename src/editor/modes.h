#ifndef __included_modes_h
#define __included_modes_h


#include <lua.h>


int mode_tiles_init ();
void mode_tiles_shutdown ();


int mode_lights_init ();
void mode_lights_shutdown ();
void mode_lights_toggle ();


int mode_objects_init ();
void mode_objects_shutdown ();
void mode_objects_register_object_hook (const char *, lua_Object,
					const char *, const char *);


#endif
