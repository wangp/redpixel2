/* messages.c - client message interface
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include <string.h>
#include <allegro.h>
#include "magic4x4.h"
#include "messages.h"
#include "store.h"
#include "textout.h"
#include "timeout.h"


#define MAX_LINES	100
#define VISIBLE_LINES	5
#define SCROLL_SPEED	5000


static char *lines[MAX_LINES];
static int total_lines_ever;
static int num_lines;
static int top_line;

static timeout_t next_scroll;


/* forward declarations */
static int ingame_messages_init (void);


int messages_init (void)
{
    if (ingame_messages_init () < 0)
	return -1;
    total_lines_ever = 0;
    top_line = 0;
    return 0;
}


void messages_shutdown (void)
{
    int i;
    for (i = 0; i < num_lines; i++)
	free (lines[i]);
    num_lines = 0;
}


static void scroll_line (void)
{
    if (num_lines > 0) {
	free (lines[0]);
	memmove (lines, lines+1, (sizeof lines[0]) * (num_lines-1));
	num_lines--;
	if (top_line > num_lines)
	    top_line = num_lines;
    }

    timeout_set (&next_scroll, SCROLL_SPEED);
}


static void push_line (const char *msg)
{
    if (num_lines == MAX_LINES)
	scroll_line ();

    lines[num_lines] = strdup (msg);
    num_lines++;

    if ((num_lines - top_line) > VISIBLE_LINES)
	top_line++;

    if (num_lines == 1)
	timeout_set (&next_scroll, SCROLL_SPEED);

    total_lines_ever++;
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


/* XXX: total_lines_ever is stupid.  Maybe use a hashing method or
 * something, or just rename this to messages_change_index.
 */
int messages_total_lines_ever (void)
{
    return total_lines_ever;
}


int messages_num_lines (void)
{
    return num_lines;
}


const char *messages_get_line (int i)
{
    return lines[i];
}



/*
 * In-game messages interface.
 *
 * This was hurriedly separated from the previous code so the line
 * between the two sections is a bit wiggly perhaps.
 */


#define MAX_INPUT_LEN	30


typedef unsigned short ucs16_t;


static ucs16_t input_line[MAX_INPUT_LEN]; /* UCS-2 string */
static int input_pos;
static int input_enabled;
static int input_blink;

static FONT *fnt;
static int text_colour;
static int input_colour;


static const char *ucs16_to_utf8 (const ucs16_t *text)
{
    return uconvert ((const char *) text, U_UNICODE, NULL, U_UTF8, 0);
}


static int ingame_messages_init (void)
{
    if (!(fnt = store_get_dat ("/basic/font/mini")))
	return -1;
    
    text_colour = makecol24 (0xaf, 0xdf, 0xaf);
    input_colour = makecol24 (0xaf, 0xaf, 0xff);

    input_enabled = 0;
    input_line[0] = 0;

    return 0;
}


#define XMARGIN	4
#define YMARGIN	2


void ingame_messages_render (BITMAP *bmp)
{
    int i, y, h;
    
    y = YMARGIN;
    h = text_height (fnt);

    text_mode (-1);

    for (i = top_line; i < num_lines; i++, y += h)
	textout_right_trans_magic (bmp, fnt, lines[i],
				   bmp->w/3 - XMARGIN, y,
				   text_colour);

    if (input_enabled) {
	textout_right_trans_magic (bmp, fnt, ucs16_to_utf8 (input_line),
				   bmp->w/3 - XMARGIN - text_length (fnt, "_"), y,
				   input_colour);

	if (input_blink & 0x8)
	    textout_right_trans_magic (bmp, fnt, "_",
				       bmp->w/3 - XMARGIN, y,
				       input_colour);
	input_blink = (input_blink+1) & 0xf;
    }
}


const char *ingame_messages_poll_input (void)
{
    if ((num_lines > 0) && timeout_test (&next_scroll))
	scroll_line ();
    
    while (keypressed ()) {
	int c, sc;
	c = ureadkey (&sc);

	if (sc == KEY_ENTER) {
	    if (input_enabled) {
		input_enabled = 0;
		if (input_line[0])
		    return ucs16_to_utf8 (input_line);
	    }
	    else {
		input_line[0] = 0;
		input_pos = 0;
		input_enabled = 1;
	    }
	    continue;
	}

	if (!input_enabled)
	    continue;

	switch (sc) {

	    case KEY_ESC:
		input_enabled = 0;
		break;
		
	    case KEY_BACKSPACE:
		if (input_pos > 0)
		    input_line[--input_pos] = 0;
		break;

	    case KEY_LEFT:
	    case KEY_RIGHT:
	    case KEY_UP:
	    case KEY_DOWN:
	    case KEY_PGUP:
	    case KEY_PGDN:
	    case KEY_HOME:
	    case KEY_END:
		/* todo */
		break;

	    default:
		if ((c >= ' ') && (input_pos < MAX_INPUT_LEN - 1)) {
		    input_line[input_pos++] = c;
		    input_line[input_pos] = 0;
		}
		break;
	}
    }

    return NULL;
}


int ingame_messages_grabbed_keyboard (void)
{
    return input_enabled;
}
