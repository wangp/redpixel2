#include <string.h>
#include <allegro.h>
#include <libnet.h>
#include "client.h"
#include "clsvface.h"
#include "editor.h"
#include "error.h"
#include "extdata.h"
#include "fe-credits.h"
#include "fe-lobby.h"
#include "fe-main.h"
#include "fe-options.h"
#include "fe-widgets.h"
#include "jpgalleg.h"
#include "messages.h"
#include "music.h"
#include "server.h"
#include "store.h"
#include "sync.h"


/* XXX */
#ifdef TARGET_WINDOWS
# define INET_DRIVER	NET_DRIVER_WSOCK_WIN
#else
# define INET_DRIVER	NET_DRIVER_SOCKETS
#endif


static BITMAP *background;
static DIALOG_PLAYER *player;
static char statusbar_buf[64] = "";
static store_file_t menu_data_file;

char address_editbox_buf[32] = "localhost";
char name_editbox_buf[32] = "Gutless";
char port_editbox_buf[8] = "23415";
int client_was_kicked = 0;
int desired_menu_screen_w = 640, desired_menu_screen_h = 400;


static void *server_thread (void *arg)
{
    server_run ();
    return NULL;
}


static void select_frontend_music (void)
{
    music_select_playlist ("data/music/music-frontend.txt");
}


static void set_menu_mouse_sprite (void)
{
    set_mouse_sprite (store_get_dat ("/frontend/menu/mouse"));
    set_mouse_sprite_focus (3, 3);
}


/* Yoinked from main.c and modified. */
static int setup_video (int w, int h)
{
    int depths[] = { 16, 15, 0 }, *i;
    
    set_color_conversion (COLORCONV_NONE);
   
/*     if (d > 0) { */
/* 	set_color_depth (d); */
/* 	if (set_gfx_mode (GFX_AUTODETECT, w, h, 0, 0) == 0) */
/* 	    return 0; */
/*     } */
/*     else { */
    for (i = depths; *i; i++) {
	set_color_depth (*i);
	if (set_gfx_mode (GFX_AUTODETECT, w, h, 0, 0) == 0)
	    return 0;
    }
/*     } */
    
    return -1;
}


int set_menu_gfx_mode (void)
{
    if ((SCREEN_W != desired_menu_screen_w) ||
	(SCREEN_H != desired_menu_screen_h)) {
	if (setup_video (desired_menu_screen_w, desired_menu_screen_h) < 0) {
	    printf ("Error setting video mode.\n%s\n", allegro_error);
/* 	    errorv ("Error setting video mode.\n%s\n", allegro_error); */
	    return -1;
	}
    }

    return 0;
}


void set_menu_mouse_range (void)
{
    set_mouse_range (0, 0, desired_menu_screen_w-1, desired_menu_screen_h-1);
}


/*------------------------------------------------------------*/
/* Multiplayer menu choices                                   */
/*------------------------------------------------------------*/

/* Client connecting dialog. */
#define CONNECTING_DLG_DEFAULT_FOCUS	2
#define CONNECTING_DLG_BACKDROP		(connecting_dialog[0])
#define CONNECTING_DLG_LABEL		(connecting_dialog[1])
#define CONNECTING_DLG_CANCEL_BUTTON	(connecting_dialog[2])


static int clear_statusbar_buffer_and_exit (void)
{
    strcpy (statusbar_buf, "");
    return D_EXIT;
}


static DIALOG connecting_dialog[] =
{
    { fancy_bitmap_proc, 230, 290, 180, 40, 0, -1, 0, 0, 1, 0xa0, NULL, NULL, NULL }, /* 0 */
    { fancy_label_proc,  230, 290, 180, 40, 0, -1, 0, 0, 1, 0xa0, "Connecting...", NULL, NULL }, /* 1 */
    { fancy_button_proc, 270, 340, 100, 40, 0, -1, 27, D_EXIT, 0, 0x80, "Cancel", NULL, NULL }, /* 2 */
    { NULL }
};


void begin_connecting_dialog (void)
{
    player = init_dialog (connecting_dialog, CONNECTING_DLG_DEFAULT_FOCUS);
    set_menu_mouse_range ();
}


int poll_connecting_dialog (void)
{
    int ret = update_dialog (player) ? 0 : -1;

    blit_fancy_dirty_to_screen (connecting_dialog);
    return ret;
}


void end_connecting_dialog (void)
{
    shutdown_dialog (player);
}


/* Forcefully disconnected dialog (usually kicked, but not always). */
#define KICKED_DLG_DEFAULT_FOCUS	2
#define KICKED_DLG_BACKDROP		(kicked_dialog[0])
#define KICKED_DLG_LABEL		(kicked_dialog[1])
#define KICKED_DLG_OK_BUTTON		(kicked_dialog[2])

static DIALOG kicked_dialog[] =
{
    { fancy_bitmap_proc,   0,   0, 640, 480, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { fancy_label_proc,  160, 210, 320,  40, 0, -1, 0, 0, 1, 0xa0, "You were disconnected by the server.", NULL, NULL }, /* 1 */
    { fancy_button_proc, 270, 260, 100,  40, 0, -1, 27, D_EXIT, 0, 0x80, "Ok", NULL, NULL }, /* 2 */
    { NULL }
};


/* Client-server name/port screen. */
static DIALOG client_server_dialog[];
#define CLIENT_SERVER_DLG_DEFAULT_FOCUS	2
#define CLIENT_SERVER_DLG_BACKDROP	(client_server_dialog[0])
#define CLIENT_SERVER_DLG_NAME_LABEL	(client_server_dialog[1])
#define CLIENT_SERVER_DLG_NAME_EDITBOX	(client_server_dialog[2])
#define CLIENT_SERVER_DLG_PORT_LABEL	(client_server_dialog[3])
#define CLIENT_SERVER_DLG_PORT_EDITBOX	(client_server_dialog[4])
#define CLIENT_SERVER_DLG_CREATE_BUTTON	(client_server_dialog[5])
#define CLIENT_SERVER_DLG_BACK_BUTTON	(client_server_dialog[6])
#define CLIENT_SERVER_DLG_STATUSBAR	(client_server_dialog[7])


static int client_server_finished_entering_name (void)
{
    /* XXX: this should give focus to the port editbox. */
    return D_O_K;
}


static int create_server_pressed (void)
{
    strcpy (statusbar_buf, "");

    messages_init ();

    /* XXX should make server support multiple network types
       then use NET_DRIVER_LOCAL for the client */
    if ((server_init (client_server_interface, INET_DRIVER) < 0) ||
	(client_init (name_editbox_buf, INET_DRIVER, "127.0.0.1") < 0)) {
	strcpy (statusbar_buf, "A game server is already running on the same port?");
	object_message (&CLIENT_SERVER_DLG_STATUSBAR, MSG_DRAW, 0);
	return D_O_K;
    }

    server_inhibit_double_message ();
    sync_init (server_thread);

    client_run (1);

    sync_server_stop_requested ();
    sync_shutdown ();

    client_shutdown ();
    server_shutdown ();

    allegro_errno = &errno;	/* errno is thread-specific */

    messages_shutdown ();

    select_frontend_music ();

    return D_EXIT;
}


static DIALOG client_server_dialog[] =
{
    { fancy_bitmap_proc,   0,   0, 640, 480, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { fancy_label_proc,  185, 200,  55,  20, 0, -1, 0, 0, 2, 0xa0, "Name:", NULL, NULL }, /* 1 */
    { fancy_edit_proc,   250, 190, 240,  40, 0, -1, 0, 0, sizeof name_editbox_buf - 1, 0, name_editbox_buf, NULL, client_server_finished_entering_name }, /* 2 */
    { fancy_label_proc,  120, 250, 120,  20, 0, -1, 0, 0, 2, 0xa0, "Server port:", NULL, NULL }, /* 3 */
    { fancy_edit_proc,   250, 240, 240,  40, 0, -1, 0, 0, sizeof port_editbox_buf - 1, 0, port_editbox_buf, NULL, create_server_pressed }, /* 4 */
    { fancy_button_proc, 190, 290, 170,  40, 0, -1, 0, 0, 0, 0x80, "Start new server", NULL, create_server_pressed }, /* 5 */
    { fancy_button_proc, 370, 290,  80,  40, 0, -1, 27, 0, 0, 0x80, "Back", NULL, clear_statusbar_buffer_and_exit }, /* 6 */
    { fancy_label_proc,    0, 320, 640,  40, 0, -1, 0, 0, 1, 0xa0, statusbar_buf, NULL, NULL }, /* 7 */
    { d_yield_proc,        0,   0,   0,   0, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL },
    { NULL }
};


/* Client name/server screen. */
static DIALOG client_dialog[];
#define CLIENT_DLG_DEFAULT_FOCUS	2
#define CLIENT_DLG_BACKDROP		(client_dialog[0])
#define CLIENT_DLG_NAME_LABEL		(client_dialog[1])
#define CLIENT_DLG_NAME_EDITBOX		(client_dialog[2])
#define CLIENT_DLG_ADDRESS_LABEL	(client_dialog[3])
#define CLIENT_DLG_ADDRESS_EDITBOX	(client_dialog[4])
#define CLIENT_DLG_JOIN_SERVER_BUTTON	(client_dialog[5])
#define CLIENT_DLG_BACK_BUTTON		(client_dialog[6])


static int client_finished_entering_name (void)
{
    /* XXX: this should give focus to the address editbox. */
    return D_O_K;
}


static int join_server_pressed (void)
{
    messages_init ();

    if (client_init(name_editbox_buf, INET_DRIVER, address_editbox_buf) == 0) {
	sync_init (NULL);
	client_run (0);
	sync_shutdown ();
	client_shutdown ();
    }

    messages_shutdown ();
    set_menu_gfx_mode ();
    set_menu_mouse_range ();
    show_mouse (screen);
    select_frontend_music ();
    return D_EXIT;
}


static DIALOG client_dialog[] =
{
    { fancy_bitmap_proc,   0,   0, 640, 480, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { fancy_label_proc,  185, 200,  55,  20, 0, -1, 0, 0, 2, 0xa0, "Name:", NULL, NULL }, /* 1 */
    { fancy_edit_proc,   250, 190, 240,  40, 0, -1, 0, 0, sizeof name_editbox_buf - 1, 0, name_editbox_buf, NULL, client_finished_entering_name }, /* 2 */
    { fancy_label_proc,  175, 250,  65,  20, 0, -1, 0, 0, 2, 0xa0, "Server:", NULL, NULL }, /* 3 */
    { fancy_edit_proc,   250, 240, 240,  40, 0, -1, 0, 0, sizeof address_editbox_buf - 1, 0, address_editbox_buf, NULL, join_server_pressed }, /* 4 */
    { fancy_button_proc, 230, 290,  80,  40, 0, -1, 0, 0, 0, 0x80, "Join!", NULL, join_server_pressed }, /* 5 */
    { fancy_button_proc, 330, 290,  80,  40, 0, -1, 27, D_EXIT, 0, 0x80, "Back", NULL, NULL }, /* 6 */
    { d_yield_proc,        0,   0,   0,   0, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL },
    { NULL }
};


/*------------------------------------------------------------*/
/* Main menu choices                                          */
/*------------------------------------------------------------*/

/* Multiplayer menu. */
#define MULTIPLAYER_MENU_DEFAULT_FOCUS		1
#define MULTIPLAYER_MENU_BACKDROP		(multiplayer_menu[0])
#define MULTIPLAYER_MENU_CLIENT_SERVER_BUTTON	(multiplayer_menu[1])
#define MULTIPLAYER_MENU_CLIENT_BUTTON		(multiplayer_menu[2])
#define MULTIPLAYER_MENU_BACK_BUTTON		(multiplayer_menu[3])


static int client_server_button_pressed (void)
{
    fancy_do_dialog (client_server_dialog, CLIENT_SERVER_DLG_DEFAULT_FOCUS);

    /* In case the client server was stupid enough to kick itself. */
    if (client_was_kicked) {
	client_was_kicked = 0;
	set_menu_gfx_mode ();
	show_mouse (screen);
    }

    return D_REDRAW;
}


static int client_button_pressed (void)
{
    fancy_do_dialog (client_dialog, CLIENT_DLG_DEFAULT_FOCUS);

    /* Let the clients know if they were forcefully disconnected. */
    if (client_was_kicked) {
	client_was_kicked = 0;
	set_menu_gfx_mode ();
	fancy_do_dialog (kicked_dialog, KICKED_DLG_DEFAULT_FOCUS);
    }

    return D_EXIT;
}


static DIALOG multiplayer_menu[] =
{
    { fancy_bitmap_proc,   0,   0, 640, 480, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { fancy_button_proc, 250, 200, 140,  40, 0, -1, 0, 0, 0, 0x80, "Client-Server", NULL, client_server_button_pressed }, /* 1 */
    { fancy_button_proc, 250, 250, 140,  40, 0, -1, 0, 0, 0, 0x80, "Client", NULL, client_button_pressed }, /* 2 */
    { fancy_button_proc, 250, 300, 140,  40, 0, -1, 27, D_EXIT, 0, 0x80, "Back", NULL, NULL }, /* 3 */
    { d_yield_proc,        0,   0,   0,   0, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL },
    { NULL }
};


/*------------------------------------------------------------*/
/* The main menu                                              */
/*------------------------------------------------------------*/

static DIALOG main_menu[];
#define MAIN_MENU_DEFAULT_FOCUS		1
#define MAIN_MENU_BACKDROP		(main_menu[0])
#define MAIN_MENU_MULTIPLAYER_BUTTON	(main_menu[1])
#define MAIN_MENU_OPTIONS_BUTTON	(main_menu[2])
#define MAIN_MENU_EDITOR_BUTTON		(main_menu[3])
#define MAIN_MENU_CREDITS_BUTTON	(main_menu[4])
#define MAIN_MENU_EXIT_BUTTON		(main_menu[5])


static int multiplayer_pressed (void)
{    
    fancy_do_dialog (multiplayer_menu, MULTIPLAYER_MENU_DEFAULT_FOCUS);
    return D_REDRAW;
}


static int options_pressed (void)
{
    options_menu_run ();
    show_mouse (screen);
    return D_REDRAW;
}


static int editor_pressed (void)
{
    music_stop_playlist ();

    if (editor_init () == 0) {
	editor_run ();
	editor_shutdown ();

	set_menu_mouse_sprite ();
	show_mouse (screen);
    }

    select_frontend_music ();

    return D_REDRAW;
}


static int credits_pressed (void)
{
    scare_mouse ();
    do_credits ();
    unscare_mouse ();
    select_frontend_music ();
    return D_REDRAW;
}


static DIALOG main_menu[] =
{
    { fancy_bitmap_proc,   0,   0, 640, 480, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { fancy_button_proc, 250, 150, 140,  40, 0, -1, 0, 0, 0, 0x80, "Multiplayer", NULL, multiplayer_pressed }, /* 1 */
    { fancy_button_proc, 250, 200, 140,  40, 0, -1, 0, 0, 0, 0x80, "Options", NULL, options_pressed }, /* 2 */
    { fancy_button_proc, 250, 250, 140,  40, 0, -1, 0, 0, 0, 0x80, "Editor", NULL, editor_pressed }, /* 3 */
    { fancy_button_proc, 250, 300, 140,  40, 0, -1, 0, 0, 0, 0x80, "Credits", NULL, credits_pressed }, /* 4 */
    { fancy_button_proc, 250, 350, 140,  40, 0, -1, 0, D_EXIT, 0, 0x80, "Exit", NULL, NULL }, /* 5 */
    { d_yield_proc,        0,   0,   0,   0, 0,  0, 27, 0, 0, 0, NULL, NULL, NULL },
    { NULL }
};


/*------------------------------------------------------------*/

/* Stretches dialogs so they fit goodly in 640x400 and 640x480. */
void resize_dialog (DIALOG *d)
{
    int i = 0;

    do {
	if (d[i].y < 0)
	    d[i].y += SCREEN_H;
	if (d[i].h < 0)
	    d[i].h += SCREEN_H - d[i].y;
    } while (d[++i].proc);
}


int initialize_fancy_dialog (DIALOG *d, BITMAP *bmp, int fg, int bg)
{
    int i;
    BITMAP *tick = store_get_dat ("/frontend/menu/checkbox-tick");

    /* Doesn't do list boxes since all of them are special. */
    for (i = 0; d[i].proc; i++) {
	if (d[i].proc == fancy_bitmap_proc)
	    d[i].dp = bmp;
	else if (d[i].proc == fancy_button_proc)
	    create_fancy_button (&d[i], bmp, fg, bg);
	else if (d[i].proc == fancy_checkbox_proc)
	    create_fancy_checkbox (&d[i], bmp, fg, bg, tick);
	else if (d[i].proc == fancy_edit_proc)
	    create_fancy_editbox (&d[i], bmp, fg, bg);
	else if (d[i].proc == fancy_label_proc)
	    set_fancy_label (&d[i], fg, fancy_font);
	else if (d[i].proc == fancy_radio_proc)
	    create_fancy_radio_button (&d[i], bmp, fg, bg);
	else if (d[i].proc == fancy_slider_proc)
	    create_fancy_slider (&d[i], bmp, fg, bg);
    }

    return 0;
}


void shutdown_fancy_dialog (DIALOG *d)
{
    int i;

    for (i = 0; d[i].proc; i++) {
	if (d[i].proc == fancy_bitmap_proc)
	    ;
	else if (d[i].proc == fancy_button_proc)
	    destroy_fancy_button (&d[i]);
	else if (d[i].proc == fancy_checkbox_proc)
	    destroy_fancy_checkbox (&d[i]);
	else if (d[i].proc == fancy_label_proc)
	    ;
	else if (d[i].proc == fancy_radio_proc)
	    destroy_fancy_radio_button (&d[i]);
	else if (d[i].proc == fancy_slider_proc)
	    destroy_fancy_slider (&d[i]);
    }
}


/*------------------------------------------------------------*/

void gamemenu_run (void)
{
    select_frontend_music ();
    set_menu_mouse_sprite ();
    fancy_do_dialog (main_menu, MAIN_MENU_DEFAULT_FOCUS);
}


int gamemenu_init (void)
{
    int fg = makecol (0xff, 0xff, 0xff);
    int bg = makecol (0xbf, 0x8f, 0x3f);

    menu_data_file = store_load ("data/frontend/frontend-menu.dat", "/frontend/menu/");
    if (!menu_data_file) {
	error ("Cannot load data files.  Did you download them?\n");
	return -1;
    }

    fancy_gui_init ();

    background = load_jpg ("data/frontend/frontend-menuback.jpg", NULL);
    fancy_edit_font = store_get_dat ("/frontend/menu/lucida-12");
    fancy_font = store_get_dat ("/frontend/menu/font");
    type_sound = store_get_dat ("/frontend/menu/type-sound");

    initialize_fancy_dialog (main_menu, background, fg, bg); 
    initialize_fancy_dialog (multiplayer_menu, background, fg, bg);
    options_menu_init (background);
    initialize_fancy_dialog (client_server_dialog, background, fg, bg);
    initialize_fancy_dialog (client_dialog, background, fg, bg);
    initialize_fancy_dialog (connecting_dialog, background, fg, bg);

    CONNECTING_DLG_BACKDROP.dp = create_bitmap (CONNECTING_DLG_BACKDROP.w, CONNECTING_DLG_BACKDROP.h);
    blit (background, CONNECTING_DLG_BACKDROP.dp, 
	  CONNECTING_DLG_BACKDROP.x, CONNECTING_DLG_BACKDROP.y, 0, 0, 
	  CONNECTING_DLG_BACKDROP.w, CONNECTING_DLG_BACKDROP.h);

    initialize_fancy_dialog (kicked_dialog, background, fg, bg);
    lobby_init (background);

    return 0;
}


void gamemenu_shutdown (void)
{
    shutdown_fancy_dialog (main_menu);
    shutdown_fancy_dialog (multiplayer_menu);
    options_menu_shutdown ();
    shutdown_fancy_dialog (client_server_dialog);
    shutdown_fancy_dialog (client_dialog);
    shutdown_fancy_dialog (connecting_dialog);

    if (CONNECTING_DLG_BACKDROP.dp)
	destroy_bitmap (CONNECTING_DLG_BACKDROP.dp);
    
    shutdown_fancy_dialog (kicked_dialog);
    lobby_shutdown ();
    fancy_gui_shutdown ();

    store_unload (menu_data_file);

    destroy_bitmap (background);
}
