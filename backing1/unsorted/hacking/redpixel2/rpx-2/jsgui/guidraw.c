/* guidraw.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <jsapi.h>
#include "gui.h"
#include "guiinter.h"
#include "guiwm.h"



static char *iconify_mask[];
static char *shade_mask[];

static BITMAP *iconify_bmp;
static BITMAP *shade_bmp;



/*
 *	js-C helpers
 */

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

static inline int get_obj_int(JSContext *cx, JSObject *obj, char *prop)
{
    jsval vp1;
    if (!JS_GetProperty(cx, obj, prop, &vp1))
	return 0;
    return JSVAL_TO_INT(vp1);
}

static inline char *get_obj_string(JSContext *cx, JSObject *obj, char *prop) 
{
    jsval vp1;
    if (!JS_GetProperty(cx, obj, prop, &vp1))
	return NULL;
    return JS_GetStringBytes(JS_ValueToString(cx, vp1));
}



/*
 *	drawing helpers
 */

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

static inline void mytext(BITMAP *bmp, int x1, int y1, char *string) {
    textout(bmp, __gui_font, string, x1, y1, __gui_text_colour);
}

static inline void mytext_vcentre(BITMAP *bmp, int x1, int y1, int y2, char *string) {
    textout(bmp, __gui_font, string, x1, (y1+y2-text_height(__gui_font))/2, __gui_text_colour);
}

static inline void mytext_allcentre(BITMAP *bmp, int x1, int y1, int x2, int y2, char *string) {
    textout_centre(bmp, __gui_font, string, (x1+x2)/2, (y1+y2-text_height(__gui_font))/2, __gui_text_colour);
}



/*
 *	the drawing routines
 */


static JSBool draw_filler(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int x, y, w, h;
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {
	up_box(__gui_dest, x, y, x+w-1, y+h-1);
	return JS_TRUE;
    }
    return JS_FALSE;
}


/* window (in The Paw speak, a frame) */
static JSBool draw_window(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int x, y, w, h;
    char *s;
    
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {
	s = get_obj_string(cx, obj, "label");
	
	rectfill(__gui_dest, x, y, x+w-1, y+h-1, __gui_base_colour);
	
	/* It's surprising how many people emailed me about The Paw because the frame
	 * didn't have a visually-defined ``Titlebar'' area.  :)
	 */
	up_box(__gui_dest, x, y, x+w-41, y+WM_TITLEBAR_H-1);
	mytext_vcentre(__gui_dest, x+5, y, y+WM_TITLEBAR_H, s);

	draw_sprite(__gui_dest, iconify_bmp, x+w-40, y);
	draw_sprite(__gui_dest, shade_bmp,   x+w-20, y);
	
	up_box(__gui_dest, x, y+WM_TITLEBAR_H+1, x+w-1, y+h-1);
		
	return JS_TRUE;
    }
    
    return JS_FALSE;
}


/* pushbutton */
static JSBool draw_pushbutton(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int x, y, w, h;
    char *s;
    
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {
	s = get_obj_string(cx, obj, "label");
	up_box(__gui_dest, x, y, x+w-1, y+h-1);
	mytext_allcentre(__gui_dest, x, y, x+w, y+h, s);
	return JS_TRUE;
    }
    
    return JS_FALSE;
}


/* label */
static JSBool draw_label(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int x, y, w, h, len;
    char *s;
    
    if (get_xywh(cx, obj, argv, &x, &y, &w, &h)) {
	s = get_obj_string(cx, obj, "label");
	len = MIN(text_length(__gui_font, s), w);

	switch (get_obj_int(cx, obj, "align")) {
	    case 0: mytext_vcentre(__gui_dest, x, y, y+h, s); break;	/* skip -1, to save *tiny* bit of speed */
	    case 1: mytext_vcentre(__gui_dest, (x+w-len)/2, y, y+h, s); break;
	    case 2: mytext_vcentre(__gui_dest, x+(w-len), y, y+h, s); break;
	    default: return JS_FALSE;
	}
	
	return JS_TRUE;
    }

    return JS_FALSE;
}



static JSFunctionSpec gui_native_members[] = {
    /* Name     		Function		Number of arguments */
    { "draw_filler",		draw_filler,		2 },
    { "draw_window",		draw_window,		2 },
    { "draw_pushbutton",	draw_pushbutton,	2 },
    { "draw_label",		draw_label,		2 },
    { NULL }
};



/* shutdown routine, called by atexit */
static void free_mem()
{
    destroy_bitmap(iconify_bmp);
    destroy_bitmap(shade_bmp);
}



static BITMAP *create_icon(char *mask[])
{
    int x, y, black, c;
    BITMAP *b;
    
    black = makecol(0, 0, 0);

    b = create_bitmap(20, 20);
    clear(b);
    up_box(b, 1, 0, 19, 19);
    
    for (y=0; y<16; y++) {
	for (x=0; x<16; x++) {
	    c = (mask[y][x] == 'X') ? __gui_highlight_colour : black;
	    putpixel(b, x+2, y+2, c);
	}
    }
    
    return b;
}



/* init routine */
int __gui_draw_init_hook(JSContext *cx, JSObject *native)
{
    iconify_bmp = create_icon(iconify_mask);
    shade_bmp = create_icon(shade_mask);
    if (!iconify_bmp || !shade_bmp)
	return 0;
    
    atexit(free_mem);
    
    return JS_DefineFunctions(cx, native, gui_native_members);
}




static char *iconify_mask[] = {
    "                ",
    "                ",
    "                ",
    "     XXXXXXXXXX ",
    "     XXXXXXXXXX ",
    "     X        X ",
    "     X        X ",
    "   XXXXXXX    X ",
    "   XXXXXXX    X ",
    "   X     X    X ",
    "   X     X    X ",
    "   X     XXXXXX ",
    "   X     X      ",
    "   XXXXXXX      ",
    "                ",
    "                "
};


static char *shade_mask[] = {
    "                ",
    "                ",
    "                ",
    "   XXXXXXXXXXX  ",
    "   XXXXXXXXXXX  ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "
};

