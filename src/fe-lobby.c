#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "client.h"
#include "fe-main.h"
#include "fe-widgets.h"
#include "messages.h"
#include "server.h"
#include "sound.h"
#include "store.h"
#include "svintern.h"


#define MAP_FILES	"*.pit"
#define MAP_FILE_ATTRIB	~(FA_DIREC | FA_HIDDEN)

static int messages_list_proc (int msg, DIALOG *d, int c);
static int messages_edit_proc (int msg, DIALOG *d, int c);
static int player_list_proc (int msg, DIALOG *d, int c);
static char *get_nth_filename (const char *pat, int attrib, int n, char *fn);
static int file_lister_proc (int msg, DIALOG *d, int c);
static int fancy_list_proc_with_big_font (int msg, DIALOG *d, int c);

static DIALOG_PLAYER *player;
static char chat_editbox_buf[32] = "";
static char map_name_editbox_buf[1024] = "";
char *new_input;


/*------------------------------------------------------------*/
/* Client-Server Lobby                                        */
/*------------------------------------------------------------*/

static DIALOG server_lobby[];
#define CHAT_LOG			1 /* So we can share code for server */
#define PLAYER_LISTBOX			5 /* and client lobbies, these       */
#define PLAYER_STATS_BOX		6 /* widgets have fixed positions.   */
#define SERVER_LOBBY_DEFAULT_FOCUS	12
#define SERVER_LOBBY_BACKDROP		(server_lobby[0])
#define SERVER_LOBBY_CHAT_LOG		(server_lobby[CHAT_LOG])
#define SERVER_LOBBY_CHAT_EDITBOX	(server_lobby[2])
#define SERVER_LOBBY_MAP_NAME_EDITBOX	(server_lobby[3])
#define SERVER_LOBBY_BROWSE_MAPS_BUTTON	(server_lobby[4])
#define SERVER_LOBBY_PLAYER_LISTBOX	(server_lobby[PLAYER_LISTBOX])
#define SERVER_LOBBY_PLAYER_STATS_BOX	(server_lobby[PLAYER_STATS_BOX])
#define SERVER_LOBBY_BOOT_PLAYER_BUTTON	(server_lobby[7])
#define SERVER_LOBBY_MAP_LABEL		(server_lobby[8])
#define SERVER_LOBBY_MAPS_LISTBOX	(server_lobby[9])
#define SERVER_LOBBY_SELECT_MAP_BUTTON	(server_lobby[10])
#define SERVER_LOBBY_CANCEL_MAP_BUTTON	(server_lobby[11])
#define SERVER_LOBBY_START_GAME_BUTTON	(server_lobby[12])
#define SERVER_LOBBY_DISCONNECT_BUTTON	(server_lobby[13])


static const char *get_message_history (int index, int *list_size)
{
    if (index < 0) {
	*list_size = messages_num_lines ();
	return NULL;
    }
    else
	return messages_get_line (index);
}


static void kill_trailing_whitespace (char *s)
{
    char *x;

    if (!*s) return;

    x = s + strlen (s) - 1;
    while (isspace (*x)) x--;
    *(x+1) = '\0';
}


static int send_chat_message (void)
{
    kill_trailing_whitespace (chat_editbox_buf);

    if (chat_editbox_buf[0] == '\0')
	return D_REDRAWME;

    new_input = strdup (chat_editbox_buf);
    chat_editbox_buf[0] = '\0';

    return D_REDRAWME;
}


static int set_next_map (void)
{
    char str[1024];

    sprintf (str, ",map %s", map_name_editbox_buf);
    new_input = strdup (str);
    return D_REDRAWME;
}


static int kick_player_pressed (void)
{
    char str[1024];
    client_info_t *c;

    /* XXX: don't allow kicking yourself. */
    c = client_get_nth_client_info (SERVER_LOBBY_PLAYER_LISTBOX.d2);
    sprintf (str, ",kick %ld", c->id);
    new_input = strdup (str);

    object_message (&SERVER_LOBBY_PLAYER_LISTBOX, MSG_DRAW, 0);
    object_message (&SERVER_LOBBY_PLAYER_STATS_BOX, MSG_DRAW, 0);

    return D_O_K;
}


static void enable_map_browser (void)
{
    int flags = (D_DISABLED | D_HIDDEN);

    SERVER_LOBBY_PLAYER_LISTBOX.flags |= flags;
    SERVER_LOBBY_PLAYER_STATS_BOX.flags |= flags;
    SERVER_LOBBY_BOOT_PLAYER_BUTTON.flags |= flags;
    SERVER_LOBBY_MAPS_LISTBOX.flags &=~ flags;
    SERVER_LOBBY_SELECT_MAP_BUTTON.flags &=~ flags;
    SERVER_LOBBY_CANCEL_MAP_BUTTON.flags &=~ flags;
}


static void disable_map_browser (void)
{
    int flags = (D_DISABLED | D_HIDDEN);

    SERVER_LOBBY_PLAYER_LISTBOX.flags &=~ flags;
    SERVER_LOBBY_PLAYER_STATS_BOX.flags &=~ flags;
    SERVER_LOBBY_BOOT_PLAYER_BUTTON.flags &=~ flags;
    SERVER_LOBBY_MAPS_LISTBOX.flags |= flags;
    SERVER_LOBBY_SELECT_MAP_BUTTON.flags |= flags;
    SERVER_LOBBY_CANCEL_MAP_BUTTON.flags |= flags;
}


static int browse_maps_button_pressed (void)
{
    if (SERVER_LOBBY_MAPS_LISTBOX.flags & D_HIDDEN)
	enable_map_browser ();
    else
	disable_map_browser ();

    return D_REDRAW;
}


static int selected_next_map (const char *filename)
{
    disable_map_browser ();

    sprintf (map_name_editbox_buf, "%s", filename);
    set_next_map ();

    return D_REDRAW;
}


static int select_map_button_pressed (void)
{
    int n = SERVER_LOBBY_MAPS_LISTBOX.d1;
    char str[1024];
    char *fn = get_nth_filename (MAP_FILES, MAP_FILE_ATTRIB, n, str);

    return selected_next_map (fn);
}


static void get_player_ranking (client_info_t *c, char *str)
{
    int n = 0;
    int players_above = 0, equal = 0;
    int score, sc;
    client_info_t *info;

    /* Only just entered the arena. */
    if (strcmp (c->score, "") == 0) {
	strcpy (str, "Ranking: N/A");
	return;
    }

    /* XXX */
    score = atoi (c->score);

    while ((info = client_get_nth_client_info (n))) {
	n++;

	if (strcmp (info->score, "") == 0)
	    continue;
	if (info == c)
	    continue;

	/* XXX */
	sc = atoi (info->score);

	if (sc > score)
	    players_above++;
	else if (sc == score)
	    equal = 1;
    }

    sprintf (str, "Ranking: %s%d", equal ? "=" : "", players_above + 1);
}


static const char *get_player_stats (int index, int *list_size)
{
    int n = active_dialog[PLAYER_LISTBOX].d2;
    char str[64];
    client_info_t *c;

    if (index < 0) {
	*list_size = 3;
	return NULL;
    }

    c = client_get_nth_client_info (n);
    if (!c)
	return "";

    if (index == 0)
	sprintf (str, "Name: %s", c->name);
    else if (index == 1)
	get_player_ranking (c, str);
    else if (index == 2)
	sprintf (str, "Score: %s", c->score);
#if 0
    else if (index == 3)
	sprintf (str, "Kills: %d", 1);
    else if (index == 4)
	sprintf (str, "Deaths: %d", 10);
    else if (index == 5)
	sprintf (str, "Strike rate: %f", 1.0 / 10.0);
    else
	sprintf (str, "Frags per minute: %d", 0);
#endif

    /* XXX: memory leaks */
    return strdup (str);
}


static int start_game_pressed (void)
{
    new_input = strdup (",start");
    return D_O_K;
}


/* The dimenstions of the dialog's contents.  Used when the menu is
   resized from 640x400 to 640x480. */
static struct {
    int x, y, w, h;
} server_lobby_dimensions[] = {
    {   0,    0, 640,  480 }, /* 0:  backdrop           */
    {  20,   20, 280,  -70 }, /* 1:  chat box           */
    {  20,  -60, 280,   30 }, /* 2:  chat editbox       */
    { 380,   20, 210,   30 }, /* 3:  map editbox        */
    { 595,   25,  20,   20 }, /* 4:  browse maps button */
    { 320,   60,  50, -110 }, /* 5:  player listbox     */
    { 380,   60, 240, -110 }, /* 6:  player stats box   */
    { 550, -100,  60,   30 }, /* 7:  kick button        */
    { 320,   20,  50,   30 }, /* 8:  "Map:" label       */
    { 320,   60, 300, -110 }, /* 9:  map lister         */
    { 440, -100,  80,   30 }, /* 10: select map button  */
    { 530, -100,  80,   30 }, /* 11: cancel selection   */
    { 360,  -60, 100,   40 }, /* 12: start game button  */
    { 480,  -60, 110,   40 }, /* 13: disconnect button  */
    {   0,    0,   0,    0 }
};
    

static DIALOG server_lobby[] =
{
    { fancy_bitmap_proc, 0, 0, 640, 480, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { messages_list_proc, 20, 20, 280, -70, 0, -1, 0, D_DISABLED, 0, 0, get_message_history, NULL, NULL }, /* 1 */
    { messages_edit_proc, 20, -60, 280, 30, 0, -1, 0, 0, sizeof chat_editbox_buf - 1, 0, chat_editbox_buf, NULL, send_chat_message }, /* 2 */
    { fancy_edit_proc, 380, 20, 210, 30, 0, -1, 0, 0, sizeof map_name_editbox_buf - 1, 0, map_name_editbox_buf, NULL, set_next_map }, /* 3 */
    { fancy_button_proc, 595, 25, 20, 20, 0, -1, 0, 0, 0, 0x80, "...", NULL, browse_maps_button_pressed }, /* 4 */
    { player_list_proc, 320, 60, 50, -110, 0, -1, 0, 0, 0, 0, NULL, NULL, NULL }, /* 5 */
    { fancy_list_proc_with_big_font, 380, 60, 240, -110, 0, -1, 0, D_DISABLED, 0, 0, get_player_stats, NULL, NULL }, /* 6 */
    { fancy_button_proc, 550, -100, 60, 30, 0, -1, 0, 0, 0, 0x80, "Kick", NULL, kick_player_pressed }, /* 7 */
    { fancy_label_proc, 320, 20, 50, 30, 0, -1, 0, 0, 2, 0xa0, "Map:", NULL, NULL }, /* 8 */
    { file_lister_proc, 320, 60, 300, -110, 0, -1, 0, D_DISABLED | D_HIDDEN, 0, 0, selected_next_map, NULL, NULL }, /* 9 */
    { fancy_button_proc, 440, -100, 80, 30, 0, -1, 0, D_DISABLED | D_HIDDEN, 0, 0x80, "Ok", NULL, select_map_button_pressed }, /* 10 */
    { fancy_button_proc, 530, -100, 80, 30, 0, -1, 0, D_DISABLED | D_HIDDEN, 0, 0x80, "Cancel", NULL, browse_maps_button_pressed }, /* 11 */
    { fancy_button_proc, 360, -60, 100, 40, 0, -1, 0, 0, 0, 0x80, "GO!", NULL, start_game_pressed }, /* 12 */
    { fancy_button_proc, 480, -60, 110, 40, 0, -1, 0, D_EXIT, 0, 0x80, "Disconnect", NULL, NULL }, /* 13 */
    { d_yield_proc, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, NULL, NULL, NULL },
    { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};


void begin_client_server_lobby_dialog (void)
{
    /* Sometimes the game will run in a different screen resolution to the menu.
       Make sure we set it back after we re-enter the lobby. */
    set_menu_gfx_mode ();
    set_menu_mouse_range ();

    strcpy (map_name_editbox_buf, server_next_map_file);

    player = init_dialog (server_lobby, SERVER_LOBBY_DEFAULT_FOCUS);
}


int poll_client_server_lobby_dialog (void)
{
    int ret;

    if (new_input) {
        free (new_input);
	new_input = NULL;
    }

    ret = update_dialog (player) ? 0 : -1;
    blit_fancy_dirty_to_screen (server_lobby);
    return ret;
}


void end_client_server_lobby_dialog (void)
{
    shutdown_dialog (player);
}


/*------------------------------------------------------------*/
/* Client Lobby                                               */
/*------------------------------------------------------------*/

static DIALOG client_lobby[];
#define CLIENT_LOBBY_DEFAULT_FOCUS	2
#define CLIENT_LOBBY_BACKDROP		(client_lobby[0])
#define CLIENT_LOBBY_CHAT_LOG		(client_lobby[CHAT_LOG])
#define CLIENT_LOBBY_CHAT_EDITBOX	(client_lobby[2])
#define CLIENT_LOBBY_MAP_LABEL		(client_lobby[3])
#define CLIENT_LOBBY_MAP_NAME_EDITBOX	(client_lobby[4])
#define CLIENT_LOBBY_PLAYER_LISTBOX	(client_lobby[PLAYER_LISTBOX])
#define CLIENT_LOBBY_PLAYER_STATS_BOX	(client_lobby[PLAYER_STATS_BOX])
#define CLIENT_LOBBY_DISCONNECT_BUTTON	(client_lobby[7])


/* The dimenstions of the dialog's contents.  Used when the menu is
   resized from 640x400 to 640x480. */
static struct {
    int x, y, w, h;
} client_lobby_dimensions[] = {
    {   0,   0, 640, 480 }, /* 0: backdrop          */
    {  20,  20, 280, -70 }, /* 1: Chat log          */
    {  20, -60, 280,  30 }, /* 2: Chat editbox      */
    { 320,  20,  50,  30 }, /* 3: "Map:" label      */
    { 380,  20, 240,  30 }, /* 4: map name editbox  */
    { 320,  60,  50, -70 }, /* 5: player listbox    */
    { 380,  60, 240, -70 }, /* 6: player stats box  */
    { 480, -60, 110,  40 }, /* 7: disconnect button */
    {   0,   0,   0,   0 }
};


static DIALOG client_lobby[] =
{
    { fancy_bitmap_proc, 0, 0, 640, 480, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { messages_list_proc, 20, 20, 280, -70, 0, -1, 0, D_DISABLED, 0, 0, get_message_history, NULL, NULL }, /* 1 */
    { messages_edit_proc, 20, -60, 280, 30, 0, -1, 0, 0, sizeof chat_editbox_buf - 1, 0, chat_editbox_buf, NULL, send_chat_message }, /* 2 */
    { fancy_label_proc, 320, 20, 50, 30, 0, -1, 0, 0, 2, 0xa0, "Map:", NULL, NULL }, /* 3 */
    { fancy_edit_proc, 380, 20, 240, 30, 0, -1, 0, D_DISABLED, sizeof map_name_editbox_buf - 1, 0, map_name_editbox_buf, NULL, set_next_map }, /* 4 */
    { player_list_proc, 320, 60, 50, -70, 0, -1, 0, 0, 0, 0, NULL, NULL, NULL }, /* 5 */
    { fancy_list_proc_with_big_font, 380, 60, 240, -70, 0, -1, 0, D_DISABLED, 0, 0, get_player_stats, NULL, NULL }, /* 6 */
    { fancy_button_proc, 480, -60, 110, 40, 0, -1, 0, D_EXIT, 0, 0x80, "Disconnect", NULL, NULL }, /* 7 */
    { d_yield_proc, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, NULL, NULL, NULL },
    { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};


void begin_client_lobby_dialog (void)
{
    set_menu_gfx_mode ();
    set_menu_mouse_range ();
    player = init_dialog (client_lobby, CLIENT_LOBBY_DEFAULT_FOCUS);
}


int poll_client_lobby_dialog (void)
{
    int ret;

    if (new_input) {
        free (new_input);
	new_input = NULL;
    }

    ret = update_dialog (player) ? 0 : -1;
    blit_fancy_dirty_to_screen (client_lobby);
    return ret;
}


void end_client_lobby_dialog (void)
{
    shutdown_dialog (player);
}


const char *get_client_lobby_dialog_input (void)
{
    return new_input;
}


/*------------------------------------------------------------*/
/* Client-Server/Client Lobby init/shutdown                   */
/*------------------------------------------------------------*/

static void resize_lobby_dialogs (void)
{
    int i;

    for (i = 0; server_lobby[i].proc; i++) {
	server_lobby[i].x = server_lobby_dimensions[i].x;
	server_lobby[i].y = server_lobby_dimensions[i].y;
	server_lobby[i].w = server_lobby_dimensions[i].w;
	server_lobby[i].h = server_lobby_dimensions[i].h;
    }

    for (i = 0; client_lobby[i].proc; i++) {
	client_lobby[i].x = client_lobby_dimensions[i].x;
	client_lobby[i].y = client_lobby_dimensions[i].y;
	client_lobby[i].w = client_lobby_dimensions[i].w;
	client_lobby[i].h = client_lobby_dimensions[i].h;
    }

    resize_dialog (server_lobby);
    resize_dialog (client_lobby);
}


int lobby_init (BITMAP *background)
{
    int fg = makecol (0xff, 0xff, 0xff);
    int bg = makecol (0xbf, 0x8f, 0x3f);

    /* Cheap hack used after menus are resized. */
    if (!background)
	background = SERVER_LOBBY_BACKDROP.dp;

    resize_lobby_dialogs ();
    initialize_fancy_dialog (server_lobby, background, fg, bg);
    initialize_fancy_dialog (client_lobby, background, fg, bg);

    /* Client-Server lobby. */
    create_fancy_listbox (&SERVER_LOBBY_CHAT_LOG, background, fg, bg);
    create_fancy_editbox (&SERVER_LOBBY_CHAT_EDITBOX, background, fg, bg);
    SERVER_LOBBY_PLAYER_LISTBOX.fg = fg;
    SERVER_LOBBY_PLAYER_LISTBOX.dp2 = create_bitmap (SERVER_LOBBY_PLAYER_LISTBOX.w, SERVER_LOBBY_PLAYER_LISTBOX.h);
    SERVER_LOBBY_PLAYER_LISTBOX.dp3 = store_get_dat ("/frontend/menu/lucida-10");
    make_button_background (&SERVER_LOBBY_PLAYER_LISTBOX, SERVER_LOBBY_PLAYER_LISTBOX.dp2, background, bg);
    create_fancy_listbox (&SERVER_LOBBY_PLAYER_STATS_BOX, background, fg, bg);
    create_fancy_listbox (&SERVER_LOBBY_MAPS_LISTBOX, background, fg, bg);

    /* Client lobby. */
    create_fancy_listbox (&CLIENT_LOBBY_CHAT_LOG, background, fg, bg);
    create_fancy_editbox (&CLIENT_LOBBY_CHAT_EDITBOX, background, fg, bg);
    CLIENT_LOBBY_PLAYER_LISTBOX.fg = fg;
    CLIENT_LOBBY_PLAYER_LISTBOX.dp2 = create_bitmap (CLIENT_LOBBY_PLAYER_LISTBOX.w, CLIENT_LOBBY_PLAYER_LISTBOX.h);
    CLIENT_LOBBY_PLAYER_LISTBOX.dp3 = store_get_dat ("/frontend/menu/lucida-10");
    make_button_background (&CLIENT_LOBBY_PLAYER_LISTBOX, CLIENT_LOBBY_PLAYER_LISTBOX.dp2, background, bg);
    create_fancy_listbox (&CLIENT_LOBBY_PLAYER_STATS_BOX, background, fg, bg);
    create_fancy_button (&CLIENT_LOBBY_DISCONNECT_BUTTON, background, fg, bg);

    return 0;
}


void lobby_shutdown (void)
{
    shutdown_fancy_dialog (server_lobby);
    shutdown_fancy_dialog (client_lobby);

    /* Client-Server lobby. */
    destroy_fancy_listbox (&SERVER_LOBBY_CHAT_LOG);
    if (SERVER_LOBBY_PLAYER_LISTBOX.dp2)
	destroy_bitmap (SERVER_LOBBY_PLAYER_LISTBOX.dp2);
    destroy_fancy_listbox (&SERVER_LOBBY_PLAYER_STATS_BOX);
    destroy_fancy_listbox (&SERVER_LOBBY_MAPS_LISTBOX);

    /* Client lobby. */
    destroy_fancy_listbox (&CLIENT_LOBBY_CHAT_LOG);
    if (CLIENT_LOBBY_PLAYER_LISTBOX.dp2)
	destroy_bitmap (CLIENT_LOBBY_PLAYER_LISTBOX.dp2);
    destroy_fancy_listbox (&CLIENT_LOBBY_PLAYER_STATS_BOX);
}


/*------------------------------------------------------------*/
/* Special lobby widgets                                      */
/*------------------------------------------------------------*/

static int messages_list_proc (int msg, DIALOG *d, int c)
{
    if (msg == MSG_IDLE) {
	static int old_n = -1;
	int n = messages_total_lines_ever ();

	/* Received new text. */
	if (n != old_n) {
	    SAMPLE *samp = store_get_dat ("/frontend/menu/send-message");

	    old_n = n;
	    _fancy_list_proc (MSG_CHAR, d, (KEY_END << 8), store_get_dat ("/frontend/menu/lucida-08"));

	    play_sample (samp, 255 * sound_volume_factor, 128, 1000, FALSE);

	    return D_REDRAWME;
	}
    }

    return _fancy_list_proc (msg, d, c, store_get_dat ("/frontend/menu/lucida-08"));
}


static int messages_edit_proc (int msg, DIALOG *d, int c)
{
    if (msg == MSG_CHAR) {
	if ((c >> 8 == KEY_UP) || (c >> 8 == KEY_DOWN) ||
	    (c >> 8 == KEY_PGUP) || (c >> 8 == KEY_PGDN) ||
	    (c >> 8 == KEY_HOME) || (c >> 8 == KEY_END))
	    return messages_list_proc (msg, &active_dialog[CHAT_LOG], c);
    }

    return fancy_edit_proc (msg, d, c);
}


/* d1 stores the number of joined players, d2 the selected player, dp2
   the background, dp3 the font. */
static int player_list_proc (int msg, DIALOG *d, int c)
{
    switch (msg) {
	case MSG_START:
	    d->d1 = client_num_clients ();
	    d->d2 = 0;
	    return D_REDRAWME;

	case MSG_DRAW: {
	    int i;
	    int rtm = text_mode (d->bg);
	    FONT *fnt = d->dp3 ? d->dp3 : font;
	    int gray = makecol (0x80, 0x80, 0x80);
	    int white = makecol (0xff, 0xff, 0xff);

	    if (d->dp2)
		blit (d->dp2, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);

	    for (i = 0; i < d->d1; i++) {
		int y = d->y + 5 + i * 27;
		textprintf (fancy_screen, fnt, d->x + 5, y + 5, d->fg, "%d", i + 1);

		set_add_blender (0xff, 0xff, 0xff, 0x40);
		rectfill_wh (fancy_screen, d->x + 15, y, 30, 25, gray);
		rect_wh (fancy_screen, d->x + 15, y, 30, 25, gray);

		/* Selected. */
		if (i == d->d2) {
		    rect_wh (fancy_screen, d->x + 13, y - 2, 34, 29, white);
		    rect_wh (fancy_screen, d->x + 14, y - 1, 32, 27, white);
		    rect_wh (fancy_screen, d->x + 14, y - 1, 32, 27, white);
		}

		/* Faces :) */
		set_alpha_blender ();
		{
		    client_info_t *info = client_get_nth_client_info (i);
		    BITMAP *bmp = store_get_dat (info->face_icon);

		    if (bmp)
			draw_trans_sprite (fancy_screen, bmp, d->x + 15, y);
		}
	    }

	    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);

	    text_mode (rtm);

	    d->flags |= D_FANCY_DIRTY;
	    break;
	}

	    /* XXX: is this nogoodnik? */
	case MSG_IDLE: {
	    int old_d1 = d->d1;

	    d->d1 = client_num_clients ();
	    d->d2 = MID (0, d->d2, d->d1 - 1);

	    if (d->d1 == old_d1)
		break;

	    object_message (&active_dialog[PLAYER_STATS_BOX], MSG_DRAW, 0);
	    return D_REDRAWME;
	}

	case MSG_CLICK: {
	    int old_d2 = d->d2;

	    d->d2 = (mouse_y - d->y - 5) / 27;
	    d->d2 = MID (0, d->d2, d->d1 - 1);

	    if (d->d2 == old_d2)
		break;

	    object_message (&active_dialog[PLAYER_STATS_BOX], MSG_DRAW, 0);
	    return D_REDRAWME;
	}

	case MSG_CHAR:
	    if (c >> 8 == KEY_UP)
		d->d2--;
	    else if (c >> 8 == KEY_DOWN)
		d->d2++;
	    else
		break;

	    d->d2 = MID (0, d->d2, d->d1 - 1);
	    d->flags |= D_DIRTY;
	    return D_USED_CHAR;

	case MSG_WANTFOCUS:
	    return D_WANTFOCUS;
    }

    return D_O_K;
}


static int count_files (const char *pattern, int attrib)
{
    struct al_ffblk info;
    int n = 1;

    if (al_findfirst (pattern, &info, attrib) != 0)
	return 0;

    while (al_findnext (&info) == 0)
	n++;

    al_findclose (&info);
    return n;
}


static char *get_nth_filename (const char *pat, int attrib, int n, char *fn)
{
    struct al_ffblk info;

    if (al_findfirst (pat, &info, attrib) != 0)
	return "";

    do {
	if (n == 0)
	    break;
	n--;
    } while (al_findnext (&info) == 0);

    fn = strdup (info.name);

    al_findclose (&info);

    return fn;
}


/* d1 stores selected item, d2 top of list, dp finished proc, dp3 backdrop. */
static int file_lister_proc (int msg, DIALOG *d, int c)
{
    struct al_ffblk info;
    FONT *fnt = store_get_dat ("/frontend/menu/lucida-12");
    int height = (d->h - 10) / text_height (fnt);
    int list_size = count_files (MAP_FILES, MAP_FILE_ATTRIB);
    int my = gui_mouse_y ();
    char str[1024];

    switch (msg) {
        case MSG_DRAW: {
	    int c = makecol (0x80, 0x40, 0x40);
	    int i = 0;
	    int rtm = text_mode (d->bg);

	    /* Background. */
	    if (d->dp3)
		blit (d->dp3, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    /* The text. */
	    if (al_findfirst (MAP_FILES, &info, MAP_FILE_ATTRIB) != 0)
		break;

	    do {
		int y;

		if (i < d->d2) {
		    i++;
		    continue;
		}

		y = d->y + 5 + (i - d->d2) * text_height (fnt);

		if (i == d->d1)
		    rectfill_wh (fancy_screen, d->x + 2, y, d->w - 4, text_height (fnt), c);
		textout (fancy_screen, fnt, info.name, d->x + 5, y, d->fg);

		if (++i >= d->d2 + height)
		    break;
	    } while (al_findnext (&info) == 0);

	    al_findclose (&info);
	    text_mode (rtm);
	    d->flags |= D_FANCY_DIRTY;
	    break;
	}

	case MSG_CLICK: {
	    int old_d1 = d->d1, old_d2 = d->d2;

	    /* Scroll up. */
	    if (my < d->y + 5)
		d->d1--;
	    /* Scroll down. */
	    else if (my > d->y + 5 + height * text_height (fnt))
		d->d1++;
	    /* Selecty. */
	    else
		d->d1 = (my - d->y - 5) / text_height (fnt) + d->d2;

	    d->d1 = MID (0, d->d1, list_size - 1);
	    d->d2 = MID (d->d1 - height + 1, d->d2, d->d1);

	    if ((d->d1 != old_d1) || (d->d2 != old_d2))
		d->flags |= D_DIRTY;
	    break;
	}

        case MSG_WHEEL:
	    d->d1 -= c;
	    d->d1 = MID (0, d->d1, list_size - 1);
	    d->d2 = MID (d->d1 - height + 1, d->d2, d->d1);
	    d->flags |= D_DIRTY;
	    break;

        case MSG_DCLICK: {
	    int (*proc)(const char *) = d->dp;

	    if (!proc)
		break;

	    if ((my > d->y + 5) && (my < d->y + 5 + height * text_height (fnt)) &&
		(my < d->y + 5 + list_size * text_height (fnt)))
		return proc (get_nth_filename (MAP_FILES, MAP_FILE_ATTRIB, d->d1, str));
	}

        case MSG_CHAR:
	    if (c >> 8 == KEY_UP)
		d->d1--;
	    else if (c >> 8 == KEY_DOWN)
		d->d1++;
	    else if (c >> 8 == KEY_PGUP) {
		if (d->d1 >= d->d2)
		    d->d1 -= height - 1;
		else
		    d->d1 = d->d2;
	    }
	    else if (c >> 8 == KEY_PGDN) {
		if (d->d1 == d->d2 + height - 1)
		    d->d1 += height - 1;
		else
		    d->d1 = d->d2 + height - 1;
	    }
	    else if (c >> 8 == KEY_ENTER) {
		int (*proc)(const char *) = d->dp;

		if (proc)
		    return proc (get_nth_filename (MAP_FILES, MAP_FILE_ATTRIB, d->d1, str));
	    }
	    else
		break;

	    d->d1 = MID (0, d->d1, list_size - 1);
	    d->d2 = MID (d->d1 - height + 1, d->d2, d->d1);

	    d->flags |= D_DIRTY;
	    return D_USED_CHAR;

        case MSG_WANTFOCUS:
	    return D_WANTFOCUS;
    }

    return D_O_K;
}


static int fancy_list_proc_with_big_font (int msg, DIALOG *d, int c)
{
    return _fancy_list_proc (msg, d, c, store_get_dat ("/frontend/menu/lucida-12"));
}
