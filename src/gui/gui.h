#ifndef __included_gui_h
#define __included_gui_h


#include <allegro.h>


enum gui_event {
    GUI_EVENT_KEY_DOWN = 1,
    GUI_EVENT_KEY_UP = 2,
    GUI_EVENT_KEY_HOLD = 3,
    GUI_EVENT_KEY_TYPE = 4,

    GUI_EVENT_MOUSE_MOVE = 12,
    GUI_EVENT_MOUSE_DOWN = 13,
    GUI_EVENT_MOUSE_UP = 14,
    GUI_EVENT_MOUSE_WHEEL = 15,

    GUI_EVENT_WINDOW_GOTMOUSE = 21,
    GUI_EVENT_WINDOW_LOSTMOUSE = 22,
    GUI_EVENT_WINDOW_FOCUSED = 23,
    GUI_EVENT_WINDOW_UNFOCUSED = 24,
    GUI_EVENT_WINDOW_RAISED = 25,
    GUI_EVENT_WINDOW_LOWERED = 26,
    GUI_EVENT_WINDOW_MOVED = 27,
    GUI_EVENT_WINDOW_RESIZED = 28,
    GUI_EVENT_WINDOW_DESTROYED = 29
};

/* gui_event_t: A GUI event number.  */
typedef enum gui_event gui_event_t;


/* gui.c */

/*: Initialise the GUI subsystem.  */
int gui_init ();

/*: Shutdown the GUI subsystem.  */
void gui_shutdown ();

/*: Run the GUI subsystem.  This will not return until gui_quit is
 *  called.  See also: gui_quit.  */
void gui_main ();

/*: Stop running the GUI subsystem.  After this, you can rerun the
 *  system with gui_main, or clean up with gui_shutdown.  */
void gui_quit ();


/* guiwm.c */

typedef struct gui_wm gui_wm_t;

void gui_wm_set (gui_wm_t *);


/* gui_window_t: Opaque data type representing a window.  */
typedef struct gui_window gui_window_t;

#define GUI_HINT_NOFRAME	1
#define GUI_HINT_STEALFOCUS	2
#define GUI_HINT_GHOSTABLE	4

/*: Create a window with the specified position and dimensions.  Flags
 *  can be a combination of the following OR'd together:
 *
 *  - GUI_HINT_NOFRAME: the window will have no frame
 *
 *  - GUI_HINT_STEALFOCUS: the window steals the focus as soon as the
 *  mouse moves over it (no click required)
 *
 *  - GUI_HINT_GHOSTABLE: the user can "ghost" the window, making the
 *  window translucent, and allowing focus to pass through the window.
 */
gui_window_t *gui_window_create (int x, int y, int w, int h, int flags);

/*: Free the memory associated with a window.  */
void gui_window_destroy (gui_window_t *);

/*: Move a window to a specified position.  */
void gui_window_move (gui_window_t *, int x, int y);

/*: Move a window, relative to its current position.  */
void gui_window_move_relative (gui_window_t *, int dx, int dy);

/*: Resize a window.  */
void gui_window_resize (gui_window_t *, int w, int h);

/*: Lower a window.  */
void gui_window_lower (gui_window_t *);

/*: Raise a window.  */
void gui_window_raise (gui_window_t *);

/*: Unhide a window after being hidden.  Note that windows are not
 *  hidden by default.  */
void gui_window_show (gui_window_t *);

/*: Hide a window.  The window will disappear off the screen, and will
 *  not be able to receive focus.  */
void gui_window_hide (gui_window_t *);

/*: Mark a window as "dirty", so it will be redrawn.  */
void gui_window_dirty (gui_window_t *);

/*: Set the title of a window.  */
void gui_window_set_title (gui_window_t *, const char *title);

/*: Set the depth of a window.  Windows of a higher depth value will
 *  always appear above windows of a lower depth value.  The default
 *  depth is zero.  */
void gui_window_set_depth (gui_window_t *, int depth);

/*: Set the alpha level (translucency) of a window (0 - 255).  Windows
 *  are opaque by default (alpha value of 255).  Note that alpha
 *  levels below a certain value will be rounded up, so you cannot use
 *  zero to get a completely transparent window.   */
void gui_window_set_alpha (gui_window_t *, int alpha);

/*: Set the `self' field of a window, which is passed to draw and
 *  event procs.  By default, `self' is the window itself.  */
void gui_window_set_self (gui_window_t *, void *self);

/*: Set the callback for window redrawing.  It will be called with the
 *  window's `self' value, and the bitmap to draw to.  The bitmap is
 *  not the screen, but a private bitmap.  */
void gui_window_set_draw_proc (gui_window_t *, void (*draw) (void *self, BITMAP *));

/*: Set the callback to handle window events.  It will be called with
 *  the window's `self' value, the event number, and any extra event
 *  data.  */
void gui_window_set_event_proc (gui_window_t *, void (*event) (void *self, gui_event_t event, int d));

/*: Return the x position of a window.  */
int gui_window_x (gui_window_t *);

/*: Return the y position of a window.  */
int gui_window_y (gui_window_t *);

/*: Return the width of a window.  */
int gui_window_w (gui_window_t *);

/*: Return the height of a window.  */
int gui_window_h (gui_window_t *);

/*: Return non-zero if the window is hidden.  */
int gui_window_hidden (gui_window_t *);


/* guiaccel.c */

#define GUI_ACCEL_CTRL(x)	((x) - 'a' + 1)

/* gui_accel_t: Opaque data type representing an accelerator key.  */
typedef struct gui_accel gui_accel_t;

/*: Create an accelerator key.  KEY can be either a character, or
 *  output of the GUI_ACCEL_CTRL macro.
 *
 *  Example:  accel = gui_accel_create (GUI_ACCEL_CTRL ('z'), foo);
 */
gui_accel_t *gui_accel_create (int key, void (*proc) ());

/*: Free the memory associated with an accelerator key.  */
void gui_accel_destroy (gui_accel_t *);


/* guimouse.c */

struct gui_mouse_state {
    int  x,  y,  z;
    int dx, dy, dz;
    
    struct {
	int down, up, state;
    } b[3];
};

/* gui_mouse:
 *  This variable lets you inspect the current state of the mouse.  It
 *  contains the following read-only fields:
 *
 *  - x, y: x and y positions of the mouse cursor
 *
 *  - z: mouse wheel "position".  One 'tick' of the wheel upwards
 *  increases the value by 1.
 *
 *  - dx, dy, dz: amount of change in the x, y, z values since the
 *  last update.
 *
 *  - b[0].down, b[0].up: non-zero if the left mouse button has been
 *  pressed or released since the last update.  Note that one is the
 *  complement of the other.
 *
 *  - b[0].state: non-zero if the left mouse button is being held down
 *
 *  - b[1].down, b[1].up, b[1].state: right mouse button
 *
 *  - b[2].down, b[2].up, b[2].state: middle mouse button
 */
extern struct gui_mouse_state gui_mouse;

/*: Restrict the movement of the mouse to the confines of a window.  */
void gui_mouse_restrict (gui_window_t *);

/*: Allow the mouse to move around the entire screen).  */
void gui_mouse_unrestrict ();


#endif
