/* console.c
 *
 * Parts of this module is Unicode-unaware, as we pass strings to Lua.
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdarg.h>
#include <allegro.h>
#include <allegro/aintern.h>
#include <lauxlib.h>
#include <lua.h>
#include "alloc.h"
#include "console.h"
#include "luahelp.h"
#include "luastack.h"
#include "magic4x4.h"
#include "scripts.h"


static int __export__alert (LS);
static int __export__print (LS);


static BITMAP *background;
static int con_open;
static int con_history_lines;
static char **con_history;
static char con_line[1024];


static void scroll_history ()
{
    int i;
    
    if (!con_history[con_history_lines - 1])
	return;

    free (con_history[0]);
    
    for (i = 0; i < con_history_lines - 1; i++)
	con_history[i] = con_history[i + 1];

    con_history[i] = 0;
}


static void add (const char *s)
{
    int i;

    scroll_history ();
    
    for (i = 0; i < con_history_lines; i++) 
	if (!con_history[i]) {
	    con_history[i] = ustrdup (s);
	    break;
	}
}


static void add_history (const char *str)
{
    int i, offset = 0;

    do {
	char *s = ustrdup (str + offset);
	if (!s) break;

	for (i = 0; i < ustrlen (s); i++)
	    if (ugetat (s, i) == '\n') usetat (s, i, ' ');

	i = 0; 
	while (text_length (font, s) >= SCREEN_W)
	    usetat (s, --i, 0);

	add (s);
	offset += ustrsize (s);
        free (s);
    } while (i < 0);
}


static void clear_line (char *s)
{
    s[0] = 0;
}


int console_init ()
{
    lua_State *L = lua_state;
    BITMAP *tmp;

    tmp = load_bitmap ("data/console.bmp", 0); /* XXX: path */
    if (tmp) {
	background = get_magic_bitmap_format (tmp, 0);
	if (background)
	    set_magic_bitmap_brightness (background, 0xf, 0xf, 0xf);
	destroy_bitmap (tmp);
    }    

    con_open = 0;
    clear_line (con_line);

    con_history_lines = ((SCREEN_H / 2) / text_height (font)) - 1;   /* XXX */
    con_history = alloc (sizeof (char *) * con_history_lines);

    lua_set_alert (L, __export__alert);
    lua_register (L, "print", __export__print);

    return 0;
}


void console_shutdown ()
{
    int i;

    for (i = 0; i < con_history_lines; i++)
	free (con_history[i]);
    free (con_history);

    destroy_bitmap (background);
}


void console_open ()
{
    con_open = 1;
}


void console_close ()
{
    con_open = 0;
}


void console_printf (const char *fmt, ...)
{
    char tmp[1024];
    va_list ap;

    va_start (ap, fmt);
    uvsprintf (tmp, fmt, ap);	/* XXX: overrun? */
    va_end (ap);

    add_history (tmp);
}


void console_draw (BITMAP *buf)
{
    char tmp[1024];
    int i;
    const int white = makecol24 (255, 255, 255);
    
    if (!con_open) return;

    if (background)
	blit (background, buf, 0, 0, 0, 0, buf->w, buf->h / 2);
    else
	rectfill (buf, 0, 0, buf->w, buf->h / 2, 0);

    for (i = 0; i < con_history_lines; i++)
	if (con_history[i])
	    textprintf (buf, font, 0, text_height (font) * i, white, "%s", con_history[i]);

    textprintf (buf, font, 0, (buf->h / 2) - text_height (font), white, "$ %s_",
		uconvert_toascii (con_line, tmp));
}


static void execute (LS, const char *str)
{
    lua_getglobal (L, str);
    /* Global str now on top of stack.  */
    
    /* Call if it is a function.  */
    if (lua_isfunction (L, -1))
	lua_call (L, 0, 0);

    /* Print value if it is a variable.  */
    else if (!lua_isnil (L, -1)) {
	lua_getglobal (L, "print");
	lua_settop (L, -2);
	lua_call (L, 1, 0);
    }

    /* Else evaluate as is.  */
    else lua_dostring (L, con_line);
}


void console_update ()
{
    lua_State *L = lua_state;
    int k;

    if (!keypressed ())
	return;
    
    k = readkey();

    if ((k >> 8) == KEY_ESC) {
	if (con_open)
	    console_close ();
	else
	    console_open ();
	return;
    }

    if (!con_open)
	return;

    switch (k >> 8) {
	case KEY_ENTER: {
	    char tmp[1024];
	    add_history (uconvert_ascii (con_line, tmp));
	    execute (L, con_line);
	    clear_line (con_line);
	    break;
	}

	case KEY_BACKSPACE:
	    if (strlen (con_line) > 0)
	    	con_line[strlen (con_line) - 1] = '\0';
	    break;

	default: {
	    char str[] = { k & 0xff, 0 };
	    ustrncat (con_line, str, sizeof con_line);
	    break;
	}
    }
}



/*----------------------------------------------------------------------*/


/* We override the _ALERT function, so that the output appears in the
 * console history and not stdout.  */
static int __export__alert (LS)
{
    add_history (lua_tostring (L, 1));
    return 0;
}


/* We override Lua's print builtin, so that the output appears in the
 * console history and not stdout.  Original function header:
 *
 * If your system does not support `stdout', you can just remove this function.
 * If you need, you can define your own `print' function, following this
 * model but changing `fputs' to put the strings at a proper place
 * (a console window or a log file, for instance).
 */
static int __export__print (LS) {
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    const char *s;
    lua_pushvalue(L, -1);  /* function to be called */
    lua_pushvalue(L, i);   /* value to print */
    lua_rawcall(L, 1, 1);
    s = lua_tostring(L, -1);  /* get result */
    if (s == NULL)
      lua_error(L, "`tostring' must return a string to `print'");
/*      if (i>1) fputs("\t", stdout); */
/*      fputs(s, stdout); */
    add_history(s);
    lua_pop(L, 1);  /* pop result */
  }
/*    fputs("\n", stdout); */
  return 0;
}