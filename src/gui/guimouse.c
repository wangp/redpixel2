/* guimouse.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "guiinter.h"


struct gui_mouse_state gui_mouse;


void gui_mouse_update ()
{
    int i, mb;
    
    /* position */
    gui_mouse.dx = mouse_x - gui_mouse.x;  gui_mouse.x = mouse_x;
    gui_mouse.dy = mouse_y - gui_mouse.y;  gui_mouse.y = mouse_y;
    gui_mouse.dz = mouse_z - gui_mouse.z;  gui_mouse.z = mouse_z;
    
    /* buttons */
    mb = mouse_b;

#define HELD(x)	(mb & (1 << (x)))
    
    for (i = 0; i < 3; i++) {
	gui_mouse.b[i].down  =  HELD (i) && !gui_mouse.b[i].state;
	gui_mouse.b[i].up    = !HELD (i) &&  gui_mouse.b[i].state;
	gui_mouse.b[i].state =  HELD (i);
    }
}
