/* messages.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include <string.h>
#include <allegro.h>
#include "store.h"


#define MAX_LINES	100
#define VISIBLE_LINES	5

static char *lines[MAX_LINES];
static int num_lines;
static int top_line;

static FONT *fnt;


int messages_init ()
{
    if (!(fnt = store_dat ("/basic/font/mini")))
	return -1;
    top_line = 0;
    return 0;
}


void messages_shutdown ()
{
    int i;
    for (i = 0; i < num_lines; i++)
	free (lines[i]);
    num_lines = 0;
}


#define XMARGIN	8
#define YMARGIN	2


void messages_render (BITMAP *bmp)
{
    int i, y, h;
    
    y = YMARGIN;
    h = text_height (fnt);
    
    for (i = top_line; i < num_lines; i++, y += h)
	textout_right (bmp, fnt, lines[i], bmp->w - XMARGIN, y, -1);
}


static void push (const char *msg)
{
    if (num_lines == MAX_LINES) {
	free (lines[0]);
	memmove (lines, lines+1, (sizeof lines[0]) * (num_lines-1));
	num_lines--;
    }

    lines[num_lines] = ustrdup (msg);
    num_lines++;

    if ((num_lines - top_line) > VISIBLE_LINES)
	top_line++;
}


void messages_add (const char *fmt, ...)
{
    char buf[1024];
    va_list ap;
    
    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    va_end (ap);
    
    push (buf);
}
