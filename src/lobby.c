/* lobby.c -- trashy temporary frontend using Allegro GUI
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "lobby.h"
#include "messages.h"


static DIALOG_PLAYER *player;


/*----------------------------------------------------------------------
 * 
 * The dialog to ask which server to connect to as well as the name to
 * use.  This is a blocking dialog.
 *
 *----------------------------------------------------------------------*/

static DIALOG ask_host_and_name_dlg[] =
{
    { d_clear_proc,		/* proc */
      0, 0, 0, 0, 0, 0,		/* x y w h fg bg */
      0, 0, 0, 0,		/* key flags d1 d2 */
      NULL, NULL, NULL },	/* dp dp2 dp3 */

    { d_text_proc,
      10, 10, 80, 20, 0, 0,
      0, 0, 0, 0,
      "Connect to:", NULL, NULL },

#define DLG_HOST		2
    { d_edit_proc,
      100, 10, 150, 20, 0, 0,
      0, 0, 0, 0,
      NULL, NULL, NULL },

    { d_text_proc,
      10, 30, 80, 20, 0, 0,
      0, 0, 0, 0,
      "Name:", NULL, NULL },

#define DLG_NAME		4
    { d_edit_proc,
      100, 30, 150, 20, 0, 0,
      0, 0, 0, 0,
      NULL, NULL, NULL },

#define DLG_CONNECT_BUTTON	5
    { d_button_proc,
      10, 70, 100, 20, 0, 0,
      0, D_EXIT, 0, 0,
      "Connect", NULL, NULL },

    { NULL,
      0, 0, 0, 0, 0, 0,
      0, 0, 0, 0,
      NULL, NULL, NULL }
};

int ask_host_and_name (char *host_buf, int host_buf_size,
		       char *name_buf, int name_buf_size)
{
    DIALOG *dlg = ask_host_and_name_dlg;
    int ret;

    set_dialog_color (dlg, makecol (0, 0, 0), makecol (255, 255, 255));
    dlg[0].bg = makecol (155, 100, 155);

    dlg[DLG_HOST].dp = host_buf;
    dlg[DLG_HOST].d1 = host_buf_size;

    dlg[DLG_NAME].dp = name_buf;
    dlg[DLG_NAME].d1 = name_buf_size;

    show_mouse (screen);
    ret = do_dialog (dlg, DLG_HOST);
    show_mouse (NULL);

    return (ret == DLG_CONNECT_BUTTON) ? 0 : -1;
}



/*----------------------------------------------------------------------
 *
 * The screen the client sees during the connection process.
 *
 *----------------------------------------------------------------------*/

static DIALOG client_connect_dlg[] =
{
    { d_clear_proc,		/* proc */
      0, 0, 0, 0, 0, 0,		/* x y w h fg bg */
      0, 0, 0, 0,		/* key flags d1 d2 */
      NULL, NULL, NULL },	/* dp dp2 dp3 */

    { d_text_proc,
      10, 10, 80, 20, 0, 0,
      0, 0, 0, 0,
      "Connecting...", NULL, NULL },

    { d_button_proc,
      10, 70, 100, 20, 0, 0,
      0, D_EXIT, 0, 0,
      "Cancel", NULL, NULL },

    { NULL,
      0, 0, 0, 0, 0, 0,
      0, 0, 0, 0,
      NULL, NULL, NULL }
};

void begin_client_connecting_dialog (void)
{
    DIALOG *dlg = client_connect_dlg;

    set_dialog_color (dlg, makecol (0, 0, 0), makecol (255, 255, 255));
    dlg[0].bg = makecol (155, 100, 155);

    player = init_dialog (dlg, -1);
}

void force_redraw_client_connecting_dialog (void)
{
    broadcast_dialog_message (MSG_DRAW, 0);
}

int poll_client_connecting_dialog (void)
{
    return update_dialog (player) ? 0 : -1;
}

void end_client_connecting_dialog (void)
{
    shutdown_dialog (player);
}



/*----------------------------------------------------------------------
 * 
 * The client's lobby screen.
 * 
 *----------------------------------------------------------------------*/

static const char *messages_getter (int index, int *list_size)
{
    if (index < 0) {
	*list_size = messages_num_lines ();
	return NULL;
    }
    else {
	return messages_get_line (index);
    }
}

static char text_buf[40];
static char *new_input;

static int my_edit_proc (int msg, DIALOG *d, int c)
{
    if ((msg == MSG_CHAR) && ((c >> 8) == KEY_ENTER)) {
	new_input = text_buf;

	return D_O_K;
    }
    else {
	return d_edit_proc (msg, d, c);
    }
}

static DIALOG client_lobby_dlg[] =
{
    { d_clear_proc,		/* proc */
      0, 0, 0, 0, 0, 0,		/* x y w h fg bg */
      0, 0, 0, 0,		/* key flags d1 d2 */
      NULL, NULL, NULL },	/* dp dp2 dp3 */

#define DLG_MESSAGE_LIST 1
    { d_list_proc,
      10, 10, 300, 130, 0, 0,
      0, 0, 0, 0,
      messages_getter, NULL, NULL },

#define DLG_TEXT	2
    { my_edit_proc,
      10, 150, 300, 30, 0, 0,
      0, 0, sizeof text_buf, 0,
      text_buf, NULL, NULL },

    { d_button_proc,
      10, 170, 100, 20, 0, 0,
      27, D_EXIT, 0, 0,
      "Disconnect", NULL, NULL },

    { NULL,
      0, 0, 0, 0, 0, 0,
      0, 0, 0, 0,
      NULL, NULL, NULL }
};

void begin_client_lobby_dialog (void)
{
    DIALOG *dlg = client_lobby_dlg;

    set_dialog_color (dlg, makecol (0, 0, 0), makecol (255, 255, 255));
    dlg[0].bg = makecol (155, 100, 155);

    player = init_dialog (dlg, -1);
}

void force_redraw_client_lobby_dialog (void)
{
    DIALOG *d = client_lobby_dlg + DLG_MESSAGE_LIST;
    int n = messages_num_lines () - (d->h / text_height(font)) + 1;
    d->d2 = MAX (0, n);

    broadcast_dialog_message (MSG_DRAW, 0);
}

int poll_client_lobby_dialog (void)
{
    if (new_input) {
	new_input = NULL;
	text_buf[0] = '\0';
	object_message (client_lobby_dlg+DLG_TEXT, MSG_DRAW, 0);
    }

    return update_dialog (player) ? 0 : -1;
}

void end_client_lobby_dialog (void)
{
    shutdown_dialog (player);
}

const char *get_client_lobby_dialog_input (void)
{
    return new_input;
}