/* fps.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "fps.h"


volatile int frames, fps;

static void fps_ticker ()
{
    fps = frames;
    frames = 0;
}

END_OF_STATIC_FUNCTION (fps_ticker);


void fps_init ()
{
    LOCK_VARIABLE (frames);
    LOCK_VARIABLE (fps);
    LOCK_FUNCTION (fps_ticker);

    frames = fps = 0;
    install_int (fps_ticker, 1000);
}


void fps_shutdown ()
{
    remove_int (fps_ticker);
}
