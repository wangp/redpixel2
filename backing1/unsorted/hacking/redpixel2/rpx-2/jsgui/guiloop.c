/* guiloop.c - The mother loop
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <jsapi.h>
#include "gui.h"
#include "guiinter.h"



#define RAISE_DELAY	3



/* minfo:
 *  Easier way to find out what is happening with the mouse.
 * 
 *  Code adapted from The Paw (pmouse.cc), which was in turn "derived" from 
 *  code off the `Allegro FAQ and Techniques' web site site by Kevin Dickerson,
 *  although my version is much nicer. :)	<URL?>
 */

struct minfo {
    
    int x, y;
    
    int button[3];		       /* button up or down */

    struct mstate {
	int state;		       /* state true or false */
	int x, y;		       /* location when event occured */
    } up[3], down[3];
};



static struct minfo minfo;
static JSObject *focus = NULL;
static int focus_client = 0;

static int old_x, old_y;
static int rel_x, rel_y;



#define ENABLE_KB_HACK

#ifdef ENABLE_KB_HACK

static int _mx=0, _my=0, _mb=0;
	
/* keyboard_hack:
 *  In case you don't have a mouse (or it's broken)
 */
static void kb_hack()
{
    int speed = 5;
    
    if (mouse_x != _mx || mouse_y != _my || mouse_b) {
	_mx = mouse_x;
	_my = mouse_y;
	_mb = mouse_b;
	return;
    } 
    
    if (key_shifts & KB_SHIFT_FLAG)
	speed = 2;

    if (key[KEY_LEFT])  _mx -= speed;
    if (key[KEY_RIGHT]) _mx += speed;
    if (key[KEY_UP]) 	_my -= speed;
    if (key[KEY_DOWN])  _my += speed;
    
    if (_mx < 0) _mx = 0;
    if (_mx > SCREEN_W-1) _mx = SCREEN_W-1;
    if (_my < 0) _my = 0;
    if (_my > SCREEN_H-1) _my = SCREEN_H-1;
    
    if (_mx != mouse_x || _my != mouse_y)
        position_mouse(_mx, _my);
    
    _mb = 0;
    if (key[KEY_Z]) _mb |= 1;
    if (key[KEY_X]) _mb |= 4;
    if (key[KEY_C]) _mb |= 2;
    
    rest(1);
}

#endif /* ENABLE_KB_HACK */



/* track_minfo:
 *  Update an minfo structure with mouse events
 */
static void track_minfo(struct minfo *minfo)
{
    int i;
    
#ifdef ENABLE_KB_HACK
    kb_hack();
#endif    
        
    minfo->x = mouse_x;
    minfo->y = mouse_y;

    for (i=0; i<3; i++) {
	
#ifndef ENABLE_KB_HACK
# define _mb mouse_b
#endif	
        if (_mb & (1<<i)) {
	    if (!minfo->button[i]) {
		minfo->down[i].state = 1;
		minfo->down[i].x = minfo->x;
		minfo->down[i].y = minfo->y;
	    } 
	    else
		minfo->down[i].state = 0;
	    
	    minfo->up[i].state = 0;
	    minfo->button[i] = 1;
	}
	else {
	    if (minfo->button[i]) {
		minfo->up[i].state = 1;
		minfo->up[i].x = minfo->x;
		minfo->up[i].y = minfo->y;
	    }
	    else 
		minfo->up[i].state = 0;

	    minfo->down[i].state = 0;
	    minfo->button[i] = 0;
	}
    }
}



/* do_event:
 *  Runs an event handler.
 */
static void do_event(JSObject *obj, char *evt, jsval a, jsval b, jsval c, jsval d)
{
    jsval vp, argv[4], rval;
    
    if (!obj) 
	return;

    argv[0] = a;
    argv[1] = b;
    argv[2] = c;
    argv[3] = d;
    
    JS_GetProperty(__gui_cx, obj, evt, &vp);
    if (!JSVAL_IS_VOID(vp))
	JS_CallFunctionValue(__gui_cx, obj, vp, 4, argv, &rval);

    if (__gui_tics < RAISE_DELAY) {
	__gui_wm->raise_client(focus_client);
	__gui_tics = RAISE_DELAY;
    }
}



/* get_relative_xy:
 *  Get the __rel_x and __rel_y of a widget.
 */
static void get_relative_xy(JSObject *obj, int *rx, int *ry)
{
    jsval jx, jy;
    
    JS_GetProperty(__gui_cx, obj, "__rel_x", &jx);
    JS_GetProperty(__gui_cx, obj, "__rel_y", &jy);
    
    *rx = JSVAL_TO_INT(jx);
    *ry = JSVAL_TO_INT(jy);
}



/* gui_update:
 *  Update the GUI system, returning zero when it is time to quit.
 */
int gui_update()
{
    int i, j=0, c;
    
    if (key[KEY_ESC])
	return 0;
    
    /*
     * mouse events:
     */
    
    track_minfo(&minfo);
    
    if (focus) {
	for (i=0, j=3; i<3; i++) {
	    if (minfo.down[i].state) {
		do_event(focus, "button_down",
			 INT_TO_JSVAL(i),
			 INT_TO_JSVAL(rel_x),
			 INT_TO_JSVAL(rel_y),
			 0);
	    }
	    else if (minfo.up[i].state) {
		do_event(focus, "button_up",
			 INT_TO_JSVAL(i),
			 INT_TO_JSVAL(rel_x),
			 INT_TO_JSVAL(rel_y),
			 0);
	    }
	    else if (minfo.button[i]) {
		do_event(focus, "button_hold",
			 INT_TO_JSVAL(i),
			 INT_TO_JSVAL(rel_x),
			 INT_TO_JSVAL(rel_y),
			 0);
	    }
	    else 
		j--;
	}

	rel_x += minfo.x - old_x;
	rel_y += minfo.y - old_y;
    }
	
    if (j == 0) {
	/*
	 * This bit is *really* inefficient.
	 * 1. We get the object under focus with focus_object (slow)
	 * 2. We get the client under focus with focus_client, as an id
	 * 3. We call raise_client, which converts the id back into a wm_client_t
	 */
	
	JSObject *tmp = __gui_wm->focus_object(minfo.x, minfo.y);
	if (tmp != focus) {
	    focus = tmp;
	    if (focus) {
		c = __gui_wm->focus_client(minfo.x, minfo.y);
		focus_client = c;
		
		get_relative_xy(focus, &rel_x, &rel_y);
		__gui_tics = 0;
	    }
	}
	else {			       /* tmp == focus */
	    if (__gui_tics == RAISE_DELAY-1) {
		__gui_wm->raise_client(focus_client);
		__gui_tics = RAISE_DELAY;
	    }
	}
    }
    
    old_x = minfo.x;
    old_y = minfo.y;

    __gui_wm->update();

    return 1;
}
