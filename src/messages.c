/* messages.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include <string.h>
#include <allegro.h>
#include "magic4x4.h"
#include "store.h"


#define MAX_LINES	100
#define VISIBLE_LINES	5

static char *lines[MAX_LINES];
static int num_lines;
static int top_line;

static char input_line[512];
static int input_pos;
static int input_enabled;
static int input_blink;

static FONT *fnt;


int messages_init ()
{
    if (!(fnt = store_dat ("/basic/font/mini")))
	return -1;
    top_line = 0;
    input_enabled = 0;
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


static void textout_right_magic (BITMAP *bmp, FONT *font, const char *buf, int x, int y, int color)
{
    int len = text_length (font, buf);
    int rtm = text_mode (-1);
    BITMAP *tmp;

    tmp = create_magic_bitmap (len, text_height (font));
    clear (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_magic_sprite (bmp, tmp, x - len, y);
    destroy_bitmap (tmp);

    text_mode (rtm);
}


void messages_render (BITMAP *bmp)
{
    FONT *fnt = font;		/* XXX current font doesn't work yet */
    int i, y, h;
    
    y = YMARGIN;
    h = text_height (fnt);

    for (i = top_line; i < num_lines; i++, y += h)
	textout_right_magic (bmp, fnt, lines[i], bmp->w/3 - XMARGIN, y, -1);

    if (input_enabled) {
	textout_right_magic (bmp, fnt, input_line, bmp->w/3 - XMARGIN - 
			     text_length (fnt, "_"), y, -1);
	if ((input_blink & 0x8))
	    textout_right_magic (bmp, fnt, "_", bmp->w/3 - XMARGIN, y, -1);
	input_blink = (input_blink+1) & 0xf;
    }
}


static void push_line (const char *msg)
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
    
    push_line (buf);
}


void messages_poll_input ()
{
    int c;

    while (keypressed ()) {
	c = readkey ();

	if ((c >> 8) == KEY_ENTER) {
	    if (input_enabled) 
		input_enabled = 0;
	    else {
		usetat (input_line, 0, 0);
		input_pos = 0;
		input_enabled = 1;
	    }
	}
	else if (input_enabled) {
	    usetat (input_line, input_pos++, c & 0xff);
	    usetat (input_line, input_pos, 0);
	}
    }
}
