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
#include "timeout.h"


typedef unsigned short ucs16_t;


#define MAX_LINES	100
#define VISIBLE_LINES	5
#define MAX_INPUT_LEN	30
#define SCROLL_SPEED	5000


static char *lines[MAX_LINES];
static int num_lines;
static int top_line;

static timeout_t next_scroll;

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


int messages_init (void)
{
    if (!(fnt = store_get_dat ("/basic/font/mini")))
	return -1;
    
    text_colour = makecol24 (0xaf, 0xdf, 0xaf);
    input_colour = makecol24 (0xaf, 0xaf, 0xff);
    
    top_line = 0;
    input_enabled = 0;
    input_line[0] = 0;
    return 0;
}


void messages_shutdown (void)
{
    int i;
    for (i = 0; i < num_lines; i++)
	free (lines[i]);
    num_lines = 0;
}


#define XMARGIN	4
#define YMARGIN	2


static void textout_right_magic (BITMAP *bmp, FONT *font, const char *buf,
				 int x, int y, int color)
{
    int len = text_length (font, buf);
    int rtm = text_mode (-1);
    BITMAP *tmp;

    tmp = create_magic_bitmap (len, text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_magic_sprite (bmp, tmp, x - len, y);
    destroy_bitmap (tmp);

    text_mode (rtm);
}


void messages_render (BITMAP *bmp)
{
    int i, y, h;
    
    y = YMARGIN;
    h = text_height (fnt);

    for (i = top_line; i < num_lines; i++, y += h)
	textout_right_magic (bmp, fnt, lines[i], bmp->w/3 - XMARGIN, y,
			     text_colour);

    if (input_enabled) {
	textout_right_magic (bmp, fnt, ucs16_to_utf8 (input_line),
			     bmp->w/3 - XMARGIN - text_length (fnt, "_"), y,
			     input_colour);
	if (input_blink & 0x8)
	    textout_right_magic (bmp, fnt, "_", bmp->w/3 - XMARGIN, y,
				 input_colour);
	input_blink = (input_blink+1) & 0xf;
    }
}


static void scroll_line (void)
{
    if (num_lines > 0) {
	free (lines[0]);
	memmove (lines, lines+1, (sizeof lines[0]) * (num_lines-1));
	num_lines--;
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


const char *messages_poll_input (void)
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


int messages_grabbed_keyboard (void)
{
    return input_enabled;
}
