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
#include "luahelp.h"
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


static lua_State *lua_state;


static void list_start ()
{
    list.next = 0;
}


static void list_add_func (int ref)
{
    struct list *p = alloc (sizeof *p);

    if (p) {
	p->func = ref;
	p->next = list.next;
	list.next = p;
    }
}


static void list_run_and_end (LS)
{
    struct list *p, *next;

    for (p = list.next; p; p = next) {
	next = p->next;

	lua_getref (L, p->func);
	lua_call (L, 0, 0);
	lua_unref (L, p->func);

	free (p);
    }
}


void scripts_init ()
{
    lua_state = lua_open (0);
    lua_enablestacktraceback (lua_state);

    bindings_init ();

    list_start ();
}


void scripts_shutdown ()
{
    list_run_and_end (lua_state);
    bindings_shutdown ();
    lua_close (lua_state);
}


static void do_file (const char *filename, int attrib, int param)
{
    lua_State *L = lua_state;

    lua_pushnil (L);
    lua_setglobal (L, INIT);

    if (lua_dofile (L, filename))
	return;

    lua_getglobal (L, INIT);
    if (lua_isfunction (L, -1))
	lua_call (L, 0, 0);
    
    lua_getglobal (L, SHUTDOWN);
    if (lua_isfunction (L, -1))
	list_add_func (lua_ref (L, 1));
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
