/*
 * The mother loop.
 */


#include <allegro.h>
#include <jsapi.h>
#include "gui.h"
#include "guiinter.h"


int gui_update()
{
    if (key[KEY_ESC])
      return 0;
    
    __gui_wm->update();
        
    return 1;
}
