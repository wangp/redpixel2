/* gui.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <unistd.h>		/* XXX: for sleep */
#include <allegro.h>
#include "gui.h"
#include "guiinter.h"


static BITMAP *bmp;
static int quit;


static volatile int key_up[KEY_MAX], key_down[KEY_MAX];

static void kb_handler (int scancode)
{
    int sc = scancode & 0x7f;

    if (scancode & 0x80)  
	key_up[sc] = 1,	key_down[sc] = 0;
    else if (!key_down[sc])
	key_up[sc] = 0, key_down[sc] = 1;
}

END_OF_STATIC_FUNCTION (kb_handler);


int gui_init ()
{
    bmp = create_bitmap (SCREEN_W, SCREEN_H);
    if (!bmp)
	return -1;

    LOCK_VARIABLE (key_up);
    LOCK_VARIABLE (key_down);
    LOCK_FUNCTION (kb_handler);
    keyboard_lowlevel_callback = kb_handler;

    gui_dirty_init ();
    gui_wm_init ();

    return 0;
}


void gui_shutdown ()
{
    gui_wm_shutdown ();
    gui_dirty_shutdown ();

    keyboard_lowlevel_callback = 0;

    destroy_bitmap (bmp);
    bmp = 0;
}


static void update_screen ()
{
    gui_dirty_clear (bmp);

    if (gui_wm_update_screen (bmp)) {
	scare_mouse ();
	gui_dirty_blit (bmp, screen);
	unscare_mouse ();
    }
}


void gui_main ()
{
    int i;
    
    quit = 0;
    
    show_mouse (screen);

    while (!quit) {

	/* Keyboard events.  */
	if (keypressed ()) 
	    gui_wm_event (GUI_EVENT_KEY_TYPE, readkey ());

	for (i = 0; i < KEY_MAX; i++) {
	    if (key_down[i]) {
		if (key_down[i]  > 0)
		    gui_wm_event (GUI_EVENT_KEY_DOWN, i);
		else
		    gui_wm_event (GUI_EVENT_KEY_HOLD, i);
		key_down[i] = -1;
	    }
	    else if (key_up[i] > 0) {
		gui_wm_event (GUI_EVENT_KEY_UP, i);
		key_up[i] = -1;
	    }
	}

	/* Mouse events.  */
	gui_mouse_update();
	
	if (gui_mouse.dx || gui_mouse.dy) 
	    gui_wm_event (GUI_EVENT_MOUSE_MOVE, (gui_mouse.dx << 16) | (gui_mouse.dy));
	
	if (gui_mouse.dz) 
	    gui_wm_event (GUI_EVENT_MOUSE_WHEEL, gui_mouse.dz);

	for (i = 0; i < 3; i++) {
	    if (gui_mouse.b[i].down) 
		gui_wm_event (GUI_EVENT_MOUSE_DOWN, i);
	    else if (gui_mouse.b[i].up) 
		gui_wm_event (GUI_EVENT_MOUSE_UP, i);
	}
	
	/* Update screen.  */
	update_screen ();
		
	/* Three finger salute.  */
	if (key_shifts == (KB_CTRL_FLAG | KB_ALT_FLAG) && key[KEY_PGDN])
	    gui_quit ();

	/* Free up CPU cycles for other processes.  */
	/* XXX: not portable */
	sleep (0);
    }
    
    show_mouse (0);
}


void gui_quit ()
{
    quit = 1;
}