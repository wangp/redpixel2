/* mapobjs: map editor - object related 
 */ 

#include <stdio.h>
#include <allegro.h>

#include "mapedit.h"
#include "object.h"
#include "mapobjs.h"
#include "dirty.h"
#include "defs.h"
#include "tiles.h"
#include "rpx.h"


static int palette_top = 0;

static int selected = 0;


/* helper */
static void mark_sprite_dirty(BITMAP *sprite, int x, int y)
{
    int sx, sy, w, h;
    
    sx = x - left * TILE_W;
    sy = y - top * TILE_H;
    w = sprite->w;
    h = sprite->h;
    
    if (sx > screen_w || sy > screen_h || sx+w < 0 || sy+h < 0)
      return;
    
    mark_dirty(sx, sy, w, h);
}


/* helper */
static void draw_dirty_sprite(BITMAP *sprite, int x, int y)
{
    int sx, sy, w, h;
    
    sx = x - left * TILE_W;
    sy = y - top * TILE_H;
    w = sprite->w;
    h = sprite->h;
    
    if (sx > screen_w || sy > screen_h || sx+w < 0 || sy+h < 0)
      return;
    
    draw_sprite(dbuf, sprite, sx, sy);
    mark_dirty(sx, sy, w, h);
}


/* helper */
static object_t *get_object_under_cursor(int x, int y)
{
    object_t *it = rpx.obj_head;

    while (it) {
	
	if (it->x <= x && it->y <= y && 
	    x <= it->x + it->def->icon->w && 
	    y <= it->y + it->def->icon->h)
	  return it;
	
	it = it->next;
    }
    
    return NULL;
}


/* mouse button down */
static void mdown(int x, int y, int b)
{
}


/* dragged */
static void drag(int x, int y, int b)
{
}


/* clicked */
static void clicked(int x, int y, int b)
{
    struct object *obj;
    struct object_def *def = get_object_type(selected);
    
    if (!def)			       /* unlikely */
      return;
    
    if (b & 1)
    {
    	obj = create_object(def);
	obj->x = x;
	obj->y = y;
	
	rpx_link_object(obj);
	draw_dirty_sprite(def->icon, obj->x, obj->y);
    }
    else if (b & 2)
    {
	obj = get_object_under_cursor(x, y);
	if (!obj)
	  return;
	
	mark_sprite_dirty(obj->def->icon, obj->x, obj->y);
	rpx_unlink_object(obj);
	destroy_object(obj);
    }
}


/* draw */
static void draw(int x, int y)
{
    struct object *it = rpx.obj_head;
    
    while (it) {
	draw_dirty_sprite(it->def->icon, it->x, it->y);
	it = it->next;
    }
}


/* palette_draw:
 *  Draws the set of tools on the screen. 
 */

static void palette_draw()
{
    int x, y, max = screen_h / TILE_H;
    object_def_t *it;
    
    it = get_object_type(palette_top);
    if (!it)
      return;
    
    x = y = 0;
    
    while (it && max--) {
	
	if (it->icon)
	  stretch_sprite(palbuf, it->icon, x, y, TILE_W, TILE_H);
	
	x = (x ? 0 : TILE_W);
	if (!x)
	  y += TILE_H;
	
	it = it->next;
    }
}


/* palette_key:
 *  The palette gets a chance at the keyboard here.
 */
static void force_redraw_palette()     /* helper */
{
    palette_draw();
    force_draw_palette = 1;
    //mark_dirty(palette_x, 0, SCREEN_W-screen_w, screen_h);
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
    int u, v, i;
    struct object_def *def;

    u = x / TILE_W;
    v = y / TILE_H;
    i = palette_top + v*2 + u;
    
    def = get_object_type(i);
    if (def) 
      selected = i;
}


struct editmode mode_objects =
{
    KEY_2,
    
    mdown, 
    NULL, 
    drag,
    clicked,
    
    draw,

    palette_key,
    palette_select,
    palette_draw
};

