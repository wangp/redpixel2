/* fps.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "fps.h"


volatile int frames, fps;

static void fps_ticker (void)
{
    fps = frames;
    frames = 0;
}

END_OF_STATIC_FUNCTION (fps_ticker);


void fps_init (void)
{
    LOCK_VARIABLE (frames);
    LOCK_VARIABLE (fps);
    LOCK_FUNCTION (fps_ticker);

    frames = fps = 0;
    install_int (fps_ticker, 1000);
}


void fps_shutdown (void)
{
    remove_int (fps_ticker);
}
