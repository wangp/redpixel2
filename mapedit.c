/* Red Pixel II Map Editor
 */


#include <stdio.h>
#include <allegro.h>
#include <seer.h>

#include "defs.h"
#include "script.h"
#include "df.h"
#include "report.h"
#include "tiles.h"
#include "dirty.h"
#include "mapedit.h"
#include "maptiles.h"
#include "rpx.h"


BITMAP *dbuf;			   


int top, left;			       /* top left of screen (in tiles) */
int palette_x;			       /* where the icon palette starts */
int screen_w, screen_h;		       /* taking into account palette_x */


/* edit modes
 */
struct editmode *mode_tbl[] =
{
    &mode_tiles, 
    NULL
};


/* export_functions:
 *  Exports a few functions for use with SeeR scripts.
 */
static void export_functions()
{
    /* tiles.c */
    scLazy(create_tiles_table);
    scLazy(add_tiles_pack);
}


/* halcyon:
 *  ... + On + On.
 */

static void halcyon()
{
    struct editmode *mode, *tmp;
    int i, force_draw;
    
    top = left = 0;
    
    reset_dirty();
    not_dirty = 0;
    force_draw = 1;
    
    mode = mode_tbl[0];
 
    for (;;) {
	
	if (mouse_x < screen_w)	       /* inside editing area */
	{
	    /* scrolling */
	    if (key[KEY_A] && left > 0) { 
		left--;
		force_draw = 1;
	    }
	    
	    if (key[KEY_D]) {	    
		left++;
		force_draw = 1;
	    }
	    
	    if (key[KEY_W] && top > 0) {
		top--;
		force_draw = 1;
	    }
	    
	    if (key[KEY_S]) {
		top++;
		force_draw = 1;
	    }
	    
	    /* mouse events */
	    if (mouse_b)	
	    {
		mode->click(left*TILE_W+mouse_x, top*TILE_H+mouse_y, mouse_b);
	    }
	} else {		       /* in palette area */
	    
	    mode->palette_key();
	    
	    if (mouse_b) {
		mode->palette_click(mouse_x, mouse_y, mouse_b);
	    }
	}			    
	
	/* update screen */
	if (!not_dirty || force_draw) {
	     
	    if (force_draw)
	      mark_dirty(0, 0, screen_w, screen_h);
	    
	    clear_dirty(dbuf, NULL);

	    /* draw */
	    i = 0;
	    while ((tmp = mode_tbl[i++])) {
		tmp->draw(left, top);
	    }
	    
	    /* draw palette */
	    if (mode->palette_draw)
	      mode->palette_draw();
	    
	    show_mouse(NULL);    
	    draw_dirty(screen, dbuf);
	    show_mouse(screen);

	    force_draw = 0;
	}
	
	if (key[KEY_Q])
	  break;
    }
}


/* main:
 *  What else.
 */
int main(int argc, char **argv)
{
    int w = 400, h = 300, bpp = 16;
    
    allegro_init();
    install_keyboard();
    install_timer();
    if (install_mouse() == -1) {
	consoleprintf("Error calling install_mouse.  Perhaps you need to install a mouse driver?\n");
	return 1;
    }
    
    set_color_depth(bpp);
    if (set_gfx_mode(GFX_AUTODETECT, w, h, 0, 0) < 0) {
	consoleprintf("Couldn't set video mode %dx%dx%dbpp\n", w, h, bpp);
	return 1;
    }
    
    dbuf = create_bitmap(SCREEN_W, SCREEN_H);
    
    export_functions();
    exec_script("scripts/mapedit.sc", "init");
    
    /* defaults */
    rpx.w = 64;
    rpx.h = 64;
    
    palette_x = SCREEN_W-TILE_W;
    screen_w = palette_x;
    screen_h = SCREEN_H;
   
    halcyon();
    
    destroy_tiles_table();
    destroy_bitmap(dbuf);
    return 0;
}
