/* gameloop.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "fps.h"
#include "gameloop.h"
#include "magic4x4.h"
#include "render.h"


static volatile unsigned long tick;

static void tick_ticker ()
{
    tick++;
}

END_OF_STATIC_FUNCTION (tick_ticker);


static void tick_init (int bps)
{
    LOCK_VARIABLE (tick);
    LOCK_FUNCTION (tick_ticker);
    install_int_ex (tick_ticker, BPS_TO_TIMER (bps));
}


static void tick_shutdown ()
{
    remove_int (tick_ticker);
}


/*----------------------------------------------------------------------*/


#define TICK_RATE	60


void game_loop ()
{
    BITMAP *dbuf;
    camera_t camera;
    int white;
    int quit = 0, draw = 1;
    int t;

    dbuf = create_magic_bitmap (SCREEN_W, SCREEN_H);

    camera.x = camera.y = 0;

    white = makecol (0xff, 0xff, 0xff);

    tick_init (TICK_RATE);
    fps_init ();

    while (!quit) {
	t = tick;
	while (t > 0) {
	    /* XXX ---------- */
	    if (key[KEY_UP])    camera.y -= 2;
	    if (key[KEY_DOWN])  camera.y += 2;
	    if (key[KEY_LEFT])  camera.x -= 2;
	    if (key[KEY_RIGHT]) camera.x += 2;
	    /* end XXX ------ */

	    t--, tick--;
	    draw = 1;
	}

	if (draw) {
	    render (dbuf, map, &camera);
	    blit_magic_format (dbuf, screen, SCREEN_W, SCREEN_H);
	    textprintf (screen, font, 0, 0, white, "%d FPS", fps);

	    frames++;
	    draw = 0;
	}
	
	if (key[KEY_Q])
	    quit = 1;
    }

    fps_shutdown ();
    tick_shutdown ();

    destroy_bitmap (dbuf);
}
