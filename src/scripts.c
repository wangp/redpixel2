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


struct file {
    int shutdown;
    struct file *next;
};

static struct file file_list;


static void do_file (const char *filename, int attrib, int param)
{
    lua_Object func;
    struct file *f;

    lua_pushnil ();
    lua_setglobal (INIT);

    if (lua_dofile ((char *) filename) != 0)
	return;

    func = lua_getglobal (INIT);
    if (lua_isfunction (func))
	lua_callfunction (func);
    
    func = lua_getglobal (SHUTDOWN);
    if (!lua_isfunction (func))
	return;

    f = alloc (sizeof *f);
    if (f) {
	lua_pushobject (func);
	f->shutdown = lua_ref (1);
	f->next = file_list.next;
	file_list.next = f;
    }
}


static void undo_files ()
{
    struct file *f, *next;

    for (f = file_list.next; f; f = next) {
	next = f->next;

	lua_callfunction (lua_getref (f->shutdown));
	lua_unref (f->shutdown);
	free (f);
    }
}


void scripts_init ()
{
    lua_open ();
    lua_setdebug (lua_state, 1);
    lua_enablestacktraceback (lua_state);

    bindings_init ();

    file_list.next = 0;
}


void scripts_shutdown ()
{
    undo_files ();
    bindings_shutdown ();
    lua_close ();
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
