/* maptiles.c : mapeditor - tiles */


#include <allegro.h>

#include "mapedit.h"
#include "hash.h"
#include "dirty.h"
#include "defs.h"
#include "tiles.h"
#include "rpx.h"


static int palette_top = 0;
static int selected = 0;


/* click_handler:
 *  Handles mouse events.
 */
void click_handler(int x, int y, int b)
{
    int u, v, sx, sy;
    
    u = x / TILE_W;
    v = y / TILE_H;
    
    if (u > rpx.w || v > rpx.h)
      return;
    
    rpx.tile[v][u] = selected;
    sx = (u-left) * TILE_W;
    sy = (v-top) * TILE_H;
    draw_sprite(dbuf, tiles->tbl[selected].data, sx, sy);
    mark_dirty(sx, sy, TILE_W, TILE_H);
}


/* draw_tiles:
 */
static void draw_tiles(int x, int y)
{
    int i, xx, yy;
    int ex = screen_w / TILE_W + x,    /* end */
    	ey = screen_h / TILE_H + y;
    int yoff, xoff;
    
    yoff = 0;
    for (yy=y; yy<ey && yy<rpx.h; yy++)
    {	
	xoff = 0;	
	for (xx=x; xx<ex && xx<rpx.w; xx++) {
	    i = rpx.tile[yy][xx];
	    draw_sprite(dbuf, tiles->tbl[i].data, xoff, yoff);
	    xoff += TILE_W;
	}
	
	yoff += TILE_H;	
    }
}


/* palette_draw:
 *  Draws the set of tools on the screen. 
 */
static void palette_draw()
{
    BITMAP *bmp;
    int i = 0, yoff = 0;
    for (i = palette_top; i < tiles->size; i++)
    {
	if ((bmp = tiles->tbl[i].data)) {
	    draw_sprite(dbuf, bmp, palette_x, yoff);
	} else 
	  break;
	yoff += TILE_H;
    }
}


/* palette_key:
 *  The palette gets a chance at the keyboard here.
 */
static void force_redraw_palette()
{
    palette_draw();
    mark_dirty(palette_x, 0, SCREEN_W-screen_w, screen_h);
}

static void palette_key()
{
    if (key[KEY_W] && palette_top > 0) {
	palette_top--;
	force_redraw_palette();
    } else if (key[KEY_S]) {
	palette_top++;
	force_redraw_palette();
    }
}


/* palette_select:
 *  Allows you to click and select a particular tile type.
 */
static void palette_select(int x, int y, int b)
{
    int v = palette_top + y / TILE_H;
    
    if (v < tiles->size)
      if (tiles->tbl[v].key)
	selected = v;
}


struct editmode mode_tiles =
{
    click_handler,
    draw_tiles,

    palette_key,
    palette_select,
    palette_draw
};

