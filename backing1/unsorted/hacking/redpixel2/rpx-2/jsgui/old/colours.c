/*
 * `The Paw' style colours.
 */

#include <allegro.h>

int paw_black;
int paw_blue;
int paw_red;
int paw_green;
int paw_lightgrey;
int paw_darkgrey;

void init_thepaw_colours()
{
    paw_black		= makecol(  0,   0,   0);
    paw_blue     	= makecol(192, 192, 255);
    paw_red      	= makecol(255, 192, 192);
    paw_green    	= makecol(192, 255, 192);
    paw_lightgrey 	= makecol(0x90,0x90,0x90);
    paw_darkgrey 	= makecol(0x48,0x48,0x48);
}
