/* maptiles.c : mapeditor - tiles */

#include <allegro.h>

#include "mapedit.h"
#include "hash.h"
#include "dirty.h"
#include "defs.h"
#include "tiles.h"
#include "rpx.h"


#define SWAP(x,y)	{ int a = y; y = x; x = a; }


static int palette_top = 0;
static int selected = 0;

static int x1, y1, x2, y2, b1, blocking;


static inline void pixels2tiles(int *x, int *y)
{
    *x = *x / TILE_W;
    *y = *y / TILE_H;
}


/* mark_region:
 *  Mark a visible region of tiles as dirty.
 */
static void mark_region(int x1, int y1, int x2, int y2)
{
    int rx1, rx2, ry1, ry2;	       /* region */
    int px1, px2, py1, py2;	       /* (in pixels) */
    
    rx1 = MAX(x1, left);
    ry1 = MAX(y1, top);
    rx2 = MIN(x2+1, left+screen_w/TILE_W);
    ry2 = MIN(y2+1, top+screen_h/TILE_H);

    px1 = (rx1 - left) * TILE_W;
    px2 = (rx2 - left) * TILE_W;
    py1 = (ry1 - top) * TILE_H;
    py2 = (ry2 - top + 1) * TILE_H;

    mark_dirty(px1, py1, px2-px1, py2-py1);
}


/* set_region:
 *  Set a region of tiles to B.
 */
static void set_region(int x1, int y1, int x2, int y2, int b)
{
    int u, v;
    for (v=y1; v<=y2; v++)
      for (u=x1; u<=x2; u++)
	rpx.tile[v][u] = b;
}


/* Handle mouse events.
 */
static void mdown(int x, int y, int b)
{
    pixels2tiles(&x, &y);

    if (key_shifts & KB_SHIFT_FLAG) {
	blocking = TRUE;
	x1 = x2 = x;
	y1 = y2 = y;
	b1 = b;
    }
    else {
	int i = TILE_BLANK;
	if (b & 1)
	  i = selected;
	
	blocking = FALSE;
	set_region(x, y, x, y, i);     /* what a waste */
	mark_region(x, y, x, y);
    }
}

static void mup(int x, int y, int b)
{ 
    int i = TILE_BLANK;
    
    if (!blocking)
      return;

    pixels2tiles(&x, &y);
    x2 = x;
    y2 = y;
    
    if (x1 > x2) SWAP(x1, x2);
    if (y1 > y2) SWAP(y1, y2);
    
    if (b1 & 1) 
      i = selected;
        
    set_region(x1, y1, x2, y2, i);    
    mark_region(x1, y1, x2, y2);
}

static void drag(int x, int y, int b)
{
    int i = TILE_BLANK;
    
    if (blocking)
      return;

    if (b & 1)
      i = selected;
    
    pixels2tiles(&x, &y);
    set_region(x, y, x, y, i);     /* what a waste */
    mark_region(x, y, x, y);
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
    for (yy=y; yy<ey+1 && yy<rpx.h; yy++)
    {	
	xoff = 0;	
	for (xx=x; xx<ex && xx<rpx.w; xx++) {
	    i = rpx.tile[yy][xx];
	    if (i != TILE_BLANK)
	      draw_sprite(dbuf, tiles->tbl[i].data, xoff, yoff);
	    putpixel(dbuf, xoff, yoff, makecol(0xef,0xb0,0));
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
    int i, x, y, yoff = 0;
    
    for (y = palette_top; ; y+=2)
    {
	for (x=0; x<2; x++) 
	{
	    i = y+x;
	    if (i < tiles->size && (bmp = tiles->tbl[i].data)) {
		draw_sprite(dbuf, bmp, palette_x+x*TILE_W, yoff);
	    } else 
	      return;	    
	}	   
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
	palette_top-=2;
	force_redraw_palette();
    } else if (key[KEY_S]) {
	palette_top+=2;
	force_redraw_palette();
    }
}


/* palette_select:
 *  Allows you to click and select a particular tile type.
 */
static void palette_select(int x, int y, int b)
{
    int v = palette_top + (y / TILE_H) * 2;
    int u = (x - palette_x) / TILE_W;
    int i = u + v;
    
    if (i < tiles->size)
      if (tiles->tbl[i].key)
	selected = i;
}


struct editmode mode_tiles =
{
    KEY_1,
    
    mdown, 
    mup, 
    drag,
    NULL,
    
    draw_tiles,

    palette_key,
    palette_select,
    palette_draw
};

