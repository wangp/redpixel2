/* guitimer.c
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "guiinter.h"


#define GUI_TICRATE	4


volatile int __gui_tics = 0;


static void gui_ticker()
{
    __gui_tics++;
}

END_OF_FUNCTION(gui_ticker);



void __gui_init_ticker()
{
    LOCK_VARIABLE(__gui_tics);
    LOCK_FUNCTION(gui_ticker);
    
    install_int_ex(gui_ticker, BPS_TO_TIMER(GUI_TICRATE));
}
