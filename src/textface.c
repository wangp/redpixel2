/* textface.c - text mode interface for server
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


/* XXX this module is extremely ASCII right now.  Should probably make
   it do Unicode <-> ASCII conversions.  */


#include <ctype.h>
#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "gamesrv.h"
#include "textface.h"


static void move_to_input_cursor ();


#define CTRL(k)		(k - 'A' + 1)

static inline int min (int a, int b) { return (a < b) ? a : b; }
static inline int max (int a, int b) { return (a > b) ? a : b; }


/* colour pairs */
#define PAIR_LOG_PREFIX	1
#define PAIR_INPUT	2
#define PAIR_STATUS	3


/* Text log.  */

#define LAST_LOG_ROW	(LINES-3)

#define MAXLOGLINES	100

struct logline {
    char *prefix;
    char *text;
};

static struct logline loglines[MAXLOGLINES];
static int nloglines;

static void repaint_loglines ()
{
    int h = LAST_LOG_ROW, y, i;
    
    i = max (0, nloglines - h - 1);
	
    for (y = 0; y <= h; y++, i++) {
	move (y, 0);
	if ((i < MAXLOGLINES) && loglines[i].prefix) {
	    attrset (A_BOLD | COLOR_PAIR (PAIR_LOG_PREFIX));
	    addstr (loglines[i].prefix);
	    addstr (": ");
	}
	if ((i < MAXLOGLINES) && loglines[i].text) {
	    attrset (A_NORMAL);
	    addstr (loglines[i].text);
	}
	clrtoeol ();
    }
}

static void add_logline (const char *prefix, const char *text)
{
    if (nloglines < MAXLOGLINES-1) {
	loglines[nloglines].prefix = prefix ? strdup (prefix) : NULL;
	loglines[nloglines].text = strdup (text);
	nloglines++;
    }
    else {
	free (loglines[0].prefix);
	free (loglines[0].text);
	memmove (loglines, loglines+1, (MAXLOGLINES-1) * (sizeof (struct logline)));
	loglines[nloglines].prefix = prefix ? strdup (prefix) : NULL;
	loglines[nloglines].text = strdup (text);
    }

    repaint_loglines ();
    move_to_input_cursor ();
    refresh ();
}

static void init_loglines ()
{
    memset (loglines, 0, MAXLOGLINES * (sizeof (struct logline)));
    nloglines = 0;
}

static void end_loglines ()
{
    int i;

    for (i = 0; i < nloglines; i++) {
	free (loglines[i].prefix);
	free (loglines[i].text);
    }
}


/* Status line.  */

#define STATUS_ROW	(LINES-2)

static void repaint_status ()
{
    bkgdset (COLOR_PAIR (PAIR_STATUS));
    mvaddstr (STATUS_ROW, 0, "Red Pixel II server [nothing here yet]");
    clrtoeol ();
    bkgdset (A_NORMAL);
}

static void init_status ()
{
}

static void end_status ()
{
}


/* Input line.  */

#define INPUT_ROW	(LINES-1)

static char save_input[1024];
static char input[1024];
static int left, pos;

static void move_to_input_cursor ()
{
    move (INPUT_ROW, 2 + pos - left);
}

static void repaint_input ()
{
    int width = COLS-2;
    int margin = (width > 10) ? 10 : 0;

    if (pos < left) 
	left = max (0, pos - margin);
    else if (pos > left + width)
	left = max (0, pos - width + margin);

    attrset (A_NORMAL);
    mvaddstr (INPUT_ROW, 0, (left == 0) ? "> " : "< ");

    attrset (A_BOLD | COLOR_PAIR (PAIR_INPUT));
    addstr (input + left);
    clrtoeol ();
}

static void refresh_input ()
{
    repaint_input ();
    move_to_input_cursor ();
    refresh ();
}

static const char *poll_input ()
{
    int c = getch ();

    switch (c) {
	case ERR:
	    break;

	case CTRL('L'):
	    wrefresh (curscr);
	    refresh ();
	    break;

	case '\r':
	    strcpy (save_input, input);
	    pos = 0;
	    input[0] = input[1] = '\0';
	    repaint_input ();
	    return save_input;

	case CTRL('B'):
	case KEY_LEFT:
	    pos = max (0, pos-1);
	    refresh_input ();
	    break;

	case CTRL('F'):
	case KEY_RIGHT:
	    pos = min (strlen (input), pos+1);
	    refresh_input ();
	    break;
	    
	case CTRL('A'):
	case KEY_HOME:
	    pos = 0;
	    refresh_input ();
	    break;
	    
	case CTRL('E'):
	case KEY_END:
	    pos = strlen (input);
	    refresh_input ();
	    break;
	    
	case CTRL('K'):
	    input[pos] = input[pos+1] = '\0';
	    refresh_input ();
	    break;

	case CTRL('U'):
	    pos = 0;
	    input[0] = input[1] = '\0';
	    refresh_input ();
	    break;
	    
	case CTRL('D'):
	case KEY_DC:
	    memmove (input+pos, input+pos+1, strlen (input+pos+1) + 1);
	    refresh_input ();
	    break;

	case CTRL('H'):
	case KEY_BACKSPACE:
	case 0x7F:
	    if (pos > 0) {
		memmove (input+pos-1, input+pos, strlen (input+pos) + 1);
		pos--;
		refresh_input ();
	    }
	    break;
	    
	default:
	    if (isprint (c) && (strlen (input) + 1 < (sizeof input))) {
		memmove (input+pos+1, input+pos, strlen (input+pos) + 1);
		input[pos++] = c;
		refresh_input ();
	    }
	    break;
    }

    return NULL;
}

static void init_input ()
{
    memset (input, 0, sizeof input);
    left = 0;
    pos = 0;
}

static void end_input ()
{
}


/* Curses stuff.  */

static void (*old_sigwinch_handler)(int);

static void repaint_all ()
{
    repaint_loglines ();
    repaint_status ();
    repaint_input ();
}

static void sigwinch_handler (int num)
{
    /* pick up new screen size */
    endwin ();
    refresh ();

    repaint_all ();
    move_to_input_cursor ();
    refresh ();
}

static void init_curses ()
{
    initscr ();
    keypad (stdscr, TRUE);
    nonl ();
    nodelay (stdscr, TRUE);
    noecho ();

    if (has_colors ()) {
	start_color ();

	init_pair(PAIR_LOG_PREFIX, COLOR_RED, COLOR_BLACK);
	init_pair(PAIR_INPUT, COLOR_CYAN, COLOR_BLACK);
	init_pair(PAIR_STATUS, COLOR_WHITE, COLOR_BLUE);
    }

    old_sigwinch_handler = signal (SIGWINCH, sigwinch_handler);
}

static void end_curses ()
{
    signal (SIGWINCH, old_sigwinch_handler);
    endwin ();
}


/* External interface.  */

static int textface_init ()
{
    init_curses ();
    init_loglines ();
    init_status ();
    init_input ();

    repaint_all ();
    move_to_input_cursor ();
    refresh ();

    return 0;
}

static void textface_shutdown ()
{
    end_input ();
    end_status ();
    end_loglines ();
    end_curses ();
}

static void textface_add_log (const char *prefix, const char *text)
{
    add_logline (prefix, text);
}

static const char *textface_poll ()
{
    return poll_input ();
}

game_server_interface_t game_server_text_interface = {
    textface_init,
    textface_shutdown,
    textface_add_log,
    textface_poll
};
