/* jsgloop.c - the update / event loop (Allegro-compliant) */


#include <js/jsapi.h>
#include <allegro.h>
#include "jsgui.h"
#include "jsginter.h"


/* returns 0 when it is time to leave */
int gui_update()
{
    if (key[KEY_ESC])
      return 0;
    
    __gui_wm->update();
    
    return 1;
}
