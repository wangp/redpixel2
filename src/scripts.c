/* scripts.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <allegro.h>
#include <lua.h>
#include <luadebug.h>
#include "alloc.h"
#include "bindings.h"
#include "luastack.h"
#include "path.h"
#include "scripts.h"


#define INIT		"__module__init"
#define SHUTDOWN	"__module__shutdown"


struct list {
    int func;
    struct list *next;
};

static struct list list;


static void list_start ()
{
    list.next = 0;
}


static void list_add_func (lua_Object func)
{
    struct list *p = alloc (sizeof *p);
    
    if (p) {
	lua_pushobject (func);
	p->func = lua_ref (1);

	p->next = list.next;
	list.next = p;
    }
}


static void list_run_and_end ()
{
    struct list *p, *next;

    for (p = list.next; p; p = next) {
	next = p->next;

	lua_beginblock (); 
	lua_callfunction (lua_getref (p->func));
	lua_unref (p->func);
	lua_endblock ();

	free (p);
    }
}


void scripts_init ()
{
    lua_open ();
    lua_setdebug (lua_state, 1);
    lua_enablestacktraceback (lua_state);

    bindings_init ();

    list_start ();
}


void scripts_shutdown ()
{
    list_run_and_end ();
    bindings_shutdown ();
    lua_close ();
}


static void do_file (const char *filename, int attrib, int param)
{
    lua_Object func;

    lua_beginblock ();

    do {
	lua_pushnil ();
	lua_setglobal (INIT);

	if (lua_dofile ((char *) filename) != 0)
	    break;

	func = lua_getglobal (INIT);
	if (lua_isfunction (func))
	    lua_callfunction (func);
    
	func = lua_getglobal (SHUTDOWN);
	if (lua_isfunction (func))
	    list_add_func (func);
    } while (0);

    lua_endblock ();
}


void scripts_execute (const char *filenames)
{
    char **p, tmp[1024];

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filenames, sizeof tmp);
	for_each_file (tmp, FA_RDONLY | FA_ARCH, do_file, 0);
    }
}
