/* draw.c - native widget drawing functions
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <jsapi.h>
#include "jshelper.h"
#include "gui.h"
#include "guiinter.h"
#include "wumpus.h"
#include "native.h"



static inline void up_box(BITMAP *bmp, int x1, int y1, int x2, int y2) {
    hline(bmp, x1, y1, x2, __gui_highlight_colour);
    hline(bmp, x1, y2, x2, __gui_shadow_colour);
    vline(bmp, x1, y1, y2, __gui_highlight_colour);
    vline(bmp, x2, y1, y2, __gui_shadow_colour);
}


static inline void down_box(BITMAP *bmp, int x1, int y1, int x2, int y2) {
    hline(bmp, x1, y1, x2, __gui_shadow_colour);
    hline(bmp, x1, y2, x2, __gui_highlight_colour);
    vline(bmp, x1, y1, y2, __gui_shadow_colour);
    vline(bmp, x2, y1, y2, __gui_highlight_colour);
}



static inline void dottify_box(BITMAP *bmp, int x1, int y1, int x2, int y2) {
    int x, y;
    for (y=y1; y<=y2; y++)
	for (x=x1+(y%2); x<=x2; x+=2)
	    putpixel(bmp, x, y, __gui_shadow_colour);
}



static JSBool get_xywh(JSContext *cx, JSObject *obj, jsval *argv, 
		       int *x, int *y, int *w, int *h)
{
    jsval vp1, vp2;
    
    /* get x, y from args */
    if (!JS_ConvertArguments(cx, 2, argv, "uu", x, y))
	return JS_FALSE;

    /* get w, h from obj properties */
    if (!JS_GetProperty(cx, obj, "w", &vp1) ||
	!JS_GetProperty(cx, obj, "h", &vp2))
	return JS_FALSE;

    *w = JSVAL_TO_INT(vp1);
    *h = JSVAL_TO_INT(vp2);
    return JS_TRUE;
}



/* draw_basepalette:
 *  Say something interesting.
 */
static NATIVE_FUNCTION(draw_basepalette)
{
    /* ouch! */
    int x, y, w, h, xx, yy;
    int top, gridw, gridh;
    int ww, hh, c, selected;
    JSObject *table;
    BITMAP *bmp;
    char *key;
    jsval val;
    
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {

	top   = JS_GetPropertyInt(cx, obj, "top");
	gridw = JS_GetPropertyInt(cx, obj, "gridw");
	gridh = JS_GetPropertyInt(cx, obj, "gridh");
	selected = JS_GetPropertyInt(cx, obj, "selected");
	JS_GetProperty(cx, obj, "src", &val);
	table = JSVAL_TO_OBJECT(val);
    
	down_box(__gui_dest, x, y, x+w-1, y+h-1);

#if 0
	for (xx = x+1+gridw; xx < x+w; xx+=gridw) 
	    vline(__gui_dest, xx, y+1, y+h-2, __gui_highlight_colour);
	
	for (yy = y+1+gridh; yy < y+h; yy+=gridh) 
	    hline(__gui_dest, x+1, yy, x+w-2, __gui_highlight_colour);
#endif	
	
	/* draw sprites */
	
	ww = (int)w / gridw;
	hh = (int)h / gridh;
	
	c = top * ww;
	
	for (yy=0; yy<hh; yy++) {
	    for (xx=0; xx<ww; xx++) {
		
		if (!JS_GetElement(cx, table, c, &val) || val == JSVAL_VOID)
		    goto end;
		
		key = JS_GetStringBytes(JSVAL_TO_STRING(val));
		
		bmp = gellopy_dat(key);
		stretch_sprite(__gui_dest, bmp, x+1+xx*gridw, y+1+yy*gridh, gridw, gridh);
		
		if (c==selected) {
		    int x_ = x+1+xx*gridw;
		    int y_ = y+1+yy*gridh;
		    dottify_box(__gui_dest, x_, y_, x_+gridw-1, y_+gridh-1);
		}
		
		c++;
	    }
	}
	
	end:
	return JS_TRUE;
    }
        
    return JS_FALSE;
}



/* draw_prefabpalette:
 *  This is not going to be pretty.
 */
static NATIVE_FUNCTION(draw_prefabpalette)
{
    BITMAP *bmp, *b;
    int x, y, w, h;
    int top, gridw, gridh, num;
    int selected;
    JSObject *src, *list;
    jsval val;
    int p, xoff=0, yoff=0;
        
    if (!get_xywh(cx, obj, argv, &x, &y, &w, &h))
	return JS_FALSE;
        
    top   = JS_GetPropertyInt(cx, obj, "top");
    gridw = JS_GetPropertyInt(cx, obj, "gridw");
    gridh = JS_GetPropertyInt(cx, obj, "gridh");
    src   = JS_GetPropertyObject(cx, obj, "src");
    selected = JS_GetPropertyInt(cx, obj, "selected");
    
    down_box(__gui_dest, x, y, x+w-1, y+h-1);
    
    list = JS_GetPropertyObject(cx, src, "list");
    num = JS_GetPropertyInt(cx, list, "length");
    
    p = top*(w/gridw);
    for (; p<num; p++) {
	JSObject *prefab, *table;
	int width, height, xx, yy;
	
	if (JS_GetElement(cx, list, p, &val) == JS_FALSE)
	    break;
	prefab = JSVAL_TO_OBJECT(val);
	
	table = JS_GetPropertyObject(cx, prefab, "table");
	height = JS_GetPropertyInt(cx, prefab, "h");
	width = JS_GetPropertyInt(cx, prefab, "w");

	bmp = create_bitmap(width*16, height*16);
	clear_to_color(bmp, bitmap_mask_color(__gui_dest));
	
	for (yy=0; yy<height; yy++) {
	    JSObject *row;
	    
	    JS_GetElement(cx, table, yy, &val);
	    row = JSVAL_TO_OBJECT(val);

	    width = JS_GetPropertyInt(cx, row, "length");
	    for (xx=0; xx<width; xx++) {
		JSString *jsstr;
		char *cstr;

		if (!JS_GetElement(cx, row, xx, &val))
		    continue;
		if (JSVAL_IS_VOID(val) || JSVAL_IS_NULL(val))
		    continue;
		
		jsstr = JSVAL_TO_STRING(val);
		cstr = JS_GetStringBytes(jsstr);

		b = gellopy_dat(cstr);
		draw_sprite(bmp, b, xx*16, yy*16);
	    }
	}

	stretch_sprite(__gui_dest, bmp, x+xoff, y+yoff, gridw, gridh);
	if (p==selected) 
	    dottify_box(__gui_dest, x+xoff, y+yoff, x+xoff+gridw-1, y+yoff+gridh-1);
	
	xoff += gridw;
	if (xoff+gridh > w) {
	    xoff = 0;
	    yoff += gridh;
	    if (yoff+gridh > h)
		break;
	}
    }
    
    /*
     *  Finished?
     */
    
    return JS_TRUE;    
}



/* draw_mapviewport:
 *  Viewport of the `map'.
 */
static NATIVE_FUNCTION(draw_mapviewport)
{
    int x, y, w, h, xx, yy, ww, hh;
    int scroll_x, scroll_y;
    JSObject *src, *base, *row;
    jsval val;
    char *key;
    BITMAP *bmp;

    if (!get_xywh(cx, obj, argv, &x, &y, &w, &h))
	return JS_FALSE;
    
    scroll_x = JS_GetPropertyInt(cx, obj, "scroll_x");
    scroll_y = JS_GetPropertyInt(cx, obj, "scroll_y");

    JS_GetProperty(cx, obj, "src", &val);
    src = JSVAL_TO_OBJECT(val);

    ww = w / 16;
    hh = h / 16;
    
    
    /* draw base tiles */
    
    JS_GetProperty(cx, src, "base", &val);
    base = JSVAL_TO_OBJECT(val);
    
    for (yy=0; yy<hh; yy++) {
	if (!JS_GetElement(cx, base, yy+scroll_y, &val) || val == JSVAL_VOID)
	    continue;
	row = JSVAL_TO_OBJECT(val);
	
	for (xx=0; xx<ww; xx++) {
	    if (!JS_GetElement(cx, row, xx+scroll_x, &val) || val == JSVAL_VOID)
		continue;
	    key = JS_GetStringBytes(JSVAL_TO_STRING(val));
	    bmp = gellopy_dat(key);
	    draw_sprite(__gui_dest, bmp, x+xx*16, y+yy*16);
	}
    }
        
    /* special for "block" editing */
    do {
	int draw, x1, x2, y1, y2;
	
	draw = JS_GetPropertyInt(cx, src, "base_drawline");
	if (!draw)
	    break;
	
	x1 = JS_GetPropertyInt(cx, src, "base_x1") - scroll_x;
	y1 = JS_GetPropertyInt(cx, src, "base_y1") - scroll_y;
	x2 = JS_GetPropertyInt(cx, src, "base_x2") - scroll_x;
	y2 = JS_GetPropertyInt(cx, src, "base_y2") - scroll_y;
	
	line(__gui_dest, x+x1*16+8, y+y1*16+8, x+x2*16+8, y+y2*16+8, makecol(255,192,192));
    } while (0);
    
	
    /* draw players, sprites, etc.... 
     * this is for later 
     */
    
    return JS_TRUE;
}



/* draw_vscrollbar:
 *  (late night job, might want to fix this someday)
 */
static NATIVE_FUNCTION(draw_vscrollbar)
{
    int x, y, w, h;
    jsval vp1;
    jsdouble percent;
    int y1, y2, hh;
    
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {
	
	/* draw frame */
	up_box(__gui_dest, x, y, x+w-1, y+h-1);
	
	/* draw "bar" */
	JS_GetProperty(cx, obj, "percent", &vp1);
	JS_ValueToNumber(cx, vp1, &percent);
    	
	hh = h/4;
	y1 = y + percent * (h-hh);
	y2 = y1 + hh;
	
	if (y1 < y) {
	    y2 += (y-y1);
	    y1 = y;
	} 
	else if (y2 > y+h-1) {
	    y1 -= y2 - (y+h-1);
	    y2 = y+h-1;
	}
	
	y1+=2;			       /* force a nice border around */
	y2-=2;

	up_box(__gui_dest, x+2, y1, x+w-3, y2);

	return JS_TRUE;
    }
        
    return JS_FALSE;
}



/* draw_hscrollbar:
 *  (late night job, might want to fix this someday)
 *   oh oh, now it's been duplicated
 */
static NATIVE_FUNCTION(draw_hscrollbar)
{
    int x, y, w, h;
    jsval vp1;
    jsdouble percent;
    int x1, x2, ww;
    
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {
	
	/* draw frame */
	up_box(__gui_dest, x, y, x+w-1, y+h-1);
	
	/* draw "bar" */
	JS_GetProperty(cx, obj, "percent", &vp1);
	JS_ValueToNumber(cx, vp1, &percent);
	
	ww = w/4;
	x1 = x + percent * (w-ww);
	x2 = x1 + ww;
	
	if (x1 < x) {
	    x2 += (x-x1);
	    x1 = x;
	} 
	else if (x2 > x+w-1) {
	    x1 -= x2 - (x+w-1);
	    x2 = x+w-1;
	}
	
	x1+=2;			       /* force a nice border around */
	x2-=2;

	up_box(__gui_dest, x1, y+2, x2, y+h-3);
	
	return JS_TRUE;
    }
        
    return JS_FALSE;
}



NATIVE_FUNCTION_TABLE(draw) = {
    /* Name     		Function		Number of arguments */
    { "draw_basepalette", 	draw_basepalette,	2 },
    { "draw_prefabpalette", 	draw_prefabpalette,	2 },
    { "draw_mapviewport",  	draw_mapviewport,	2 },
    { "draw_vscrollbar",  	draw_vscrollbar,	2 },
    { "draw_hscrollbar",  	draw_hscrollbar,	2 },
    { NULL }
};



