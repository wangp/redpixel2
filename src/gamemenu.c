/* gamemenu.c - menu.c was already taken
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "editor.h"
#include "extdata.h"
#include "magic4x4.h"
#include "menu.h"
#include "store.h"


typedef enum {
    M_END,
    M_PROC,
    M_TEXT
} menu_type_t;


typedef struct menu {
    menu_type_t type;
    char *label;
    int (*proc)(void);
} menu_t;


static void run_menu (menu_t *menu);


static store_file_t menu_data_file;
static BITMAP *background;
static FONT *fnt;
static BITMAP *dbuf;



/* Generic function for backing out of a menu.  */
static int generic_back (void)
{
    return -1;
}



/*
 *----------------------------------------------------------------------
 *	Multiplayer menu
 *----------------------------------------------------------------------
 */


#include "messages.h"
#include "sync.h"
#include "client.h"
#include "libnet.h"
#include "clsvface.h"
#include "server.h"


static void *server_thread (void *arg)
{
    server_run ();
    return NULL;
}


static int mp_client_server (void)
{
    char *name = "tjaden";
    messages_init ();
	
    /* XXX should make server support multiple network types
       then use NET_DRIVER_LOCAL for the client */
    if ((server_init (client_server_interface, NET_DRIVER_SOCKETS) < 0) ||
	(client_init (name, NET_DRIVER_SOCKETS, "127.0.0.1") < 0)) {
	allegro_message (
	    "Error initialising game server or client.  Perhaps another\n"
	    "game server is already running on the same port?\n");
    } else {
	sync_init (server_thread);
	client_run (1);
	sync_server_stop_requested ();
	sync_shutdown ();

	client_shutdown ();
	server_shutdown ();

	allegro_errno = &errno;	/* errno is thread-specific */
    }

    messages_shutdown ();

    return 0;
}


static int mp_client_go (void)
{
    const char *name = "tjaden";
    const char *addr = "192.168.0.1";
    messages_init ();
    if (client_init (name, NET_DRIVER_SOCKETS, addr) == 0) {
	sync_init (NULL);
	client_run (0);
	sync_shutdown ();
	client_shutdown ();
    }
    messages_shutdown ();

    return 0;
}


static char client_connect_address[64] = "192.168.0.1";


static menu_t mp_client_menu[] =
{
    { M_TEXT, client_connect_address, 0 },
    { M_PROC, "Go!", mp_client_go },
    { M_PROC, "Back", generic_back },
    { M_END, 0, 0 }
};


static int mp_client (void)
{
    run_menu (mp_client_menu);
    return 0;
}


static menu_t mp_menu[] =
{
    { M_PROC, "Client-Server", mp_client_server },
    { M_PROC, "Client", mp_client },
    { M_PROC, "Back", generic_back },
    { M_END, 0, 0 }
};



/*
 *----------------------------------------------------------------------
 *	Options menu
 *----------------------------------------------------------------------
 */


static menu_t opt_menu[] =
{
    { M_PROC, "Video", 0 },
    { M_PROC, "Audio", 0 },
    { M_PROC, "Control", 0 },
    { M_PROC, "Network", 0 },
    { M_PROC, "Back", generic_back },
    { M_END, 0, 0 }
};



/*
 *----------------------------------------------------------------------
 *	Root menu
 *----------------------------------------------------------------------
 */


static int root_mp (void)
{
    run_menu (mp_menu);
    return 0;
}


static int root_editor (void)
{
    if (editor_init () == 0) {
	editor_run ();
	editor_shutdown ();
    }
    return 0;
}


static int root_opt (void)
{
    run_menu (opt_menu);
    return 0;
}


static menu_t root_menu[] =
{
    { M_PROC, "Multiplayer", root_mp },
    { M_PROC, "Options", root_opt },
    { M_PROC, "Editor", root_editor },
    { M_PROC, "Credits", 0 },
    { M_PROC, "Exit", generic_back },
    { M_END, 0, 0 }
};



/*
 *----------------------------------------------------------------------
 *	Menu system
 *----------------------------------------------------------------------
 */


static void textout_magic (BITMAP *bmp, FONT *font, const char *buf,
			   int x, int y, int color)
{
    int len = text_length (font, buf);
    int rtm = text_mode (-1);
    BITMAP *tmp;

    tmp = create_magic_bitmap (len, text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_magic_sprite (bmp, tmp, x, y);
    destroy_bitmap (tmp);

    text_mode (rtm);
}


#define X_CENTRE	(SCREEN_W/2)
#define Y_START		80
#define Y_SPACING	20


static void draw_menu (menu_t *menu, int selected)
{
    const int h = text_height (fnt);
    int len, x, y, i;

    /* the background */
    blit (background, dbuf, 0, 0, 0, 0, background->w, background->h);

    /* the cursor */
    if (menu[selected].label)
	len = text_length (fnt, menu[selected].label);
    else
	len = 100;
    x = X_CENTRE - len/2;
    y = Y_START + (selected * Y_SPACING);

    set_add_blender (0, 0, 0, 5);
    drawing_mode (DRAW_MODE_TRANS, 0, 0, 0);
    rectfill (dbuf, x-20, y-5, x+len+20, y+h+5, makecol24 (0xbf, 0x8f, 0x3f));
    drawing_mode (DRAW_MODE_SOLID,0,0,0);

    /* the text */
    for (i = 0; menu[i].type != M_END; i++) {
	if (menu[i].label) {
	    textout_magic (dbuf, fnt, menu[i].label,
			   X_CENTRE - text_length (fnt, menu[i].label)/2,
			   Y_START + (i * Y_SPACING),
			   makecol24 (0xef, 0xef, 0xef));
	}
    }

    /* blit to screen */
    blit_magic_format (dbuf, screen, SCREEN_W, SCREEN_H);
}


static int handle_input (menu_t *menu, int *selected)
{
    while (1) {
	switch (readkey () >> 8) {
	    case KEY_UP:
	    case KEY_W:
		if ((*selected) > 0)
		    (*selected)--;
		return 0;

	    case KEY_DOWN:
	    case KEY_S:
		if (menu[(*selected) + 1].label)
		    (*selected)++;
		return 0;

	    case KEY_ENTER:
		return (menu[*selected].proc ?
			menu[*selected].proc () : 0);

	    case KEY_ESC:
		return -1;
	}
    }
}


static void run_menu (menu_t *menu)
{
    int selected = 0;

    do draw_menu (menu, selected);
    while (handle_input (menu, &selected) == 0);
}


void gamemenu_run (void)
{
    run_menu (root_menu);
}



/*
 *----------------------------------------------------------------------
 *	Initialisation
 *----------------------------------------------------------------------
 */


int gamemenu_init (void)
{
    menu_data_file = store_load_ex ("data/frontend/frontend-menu.dat",
				    "/frontend/menu/", load_extended_datafile);
    if (!menu_data_file)
	return -1;
    
    background = store_get_dat ("/frontend/menu/background");
    set_magic_bitmap_brightness (background, 0xf, 0xf, 0xf);

    fnt = store_get_dat ("/frontend/menu/font");

    dbuf = create_magic_bitmap (SCREEN_W, SCREEN_H);

    return 0;
}


void gamemenu_shutdown (void)
{
    destroy_bitmap (dbuf);
    store_unload (menu_data_file);
}
