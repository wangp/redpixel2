#ifndef __included_ug_h
#define __included_ug_h


struct gui_window;
struct BITMAP;


/* ug.c */

/*: Initialise the UG subsystem.  */
int ug_init ();

/*: Shutdown the UG subsystem.  */
void ug_shutdown ();


/* ugwidget.c */

/*: Opaque data type representing a widget class.  */
typedef struct ug_widget_class ug_widget_class_t;

/*: Opaque data type representing a widget.  */
typedef struct ug_widget ug_widget_t;

/*: Mark a widget as "dirty", to be redrawn later.  Currently, this marks the
 *  widget's dialog as dirty, so the entire dialog is redrawn, but you should 
 *  not depend on that.  */
void ug_widget_dirty (ug_widget_t *);

/*: Give a widget input focus, within its own dialog.  This is equivalent to
 *  calling ug_dialog_focus(dialog, widget), if you know what the dialog should
 *  be.  See also: ug_dialog_focus.   */
void ug_widget_focus (ug_widget_t *);

/*: Return the absolute x position of a widget.  */
int ug_widget_x (ug_widget_t *);

/*: Return the absolute y position of a widget.  */
int ug_widget_y (ug_widget_t *);

/*: Return the width of a widget, in pixels.  */
int ug_widget_w (ug_widget_t *);

/*: Return the height of a widget, in pixels.  */
int ug_widget_h (ug_widget_t *);

/*: Return the id string of a widget.  */
char *ug_widget_id (ug_widget_t *);


/* ugevent.c */

enum ug_event {
    UG_EVENT_KEY_DOWN = 1,
    UG_EVENT_KEY_UP = 2,
    UG_EVENT_KEY_TYPE = 3,

    UG_EVENT_MOUSE_MOVE = 12,
    UG_EVENT_MOUSE_DOWN = 13,
    UG_EVENT_MOUSE_UP = 14,
    UG_EVENT_MOUSE_WHEEL = 15,

    UG_EVENT_WIDGET_GOTMOUSE = 21,
    UG_EVENT_WIDGET_LOSTMOUSE = 22,
    UG_EVENT_WIDGET_FOCUSED = 23,
    UG_EVENT_WIDGET_UNFOCUSED = 24,
    UG_EVENT_WIDGET_DRAW = 99,
};

enum ug_signal {
    /* no overlap with events, just to be safe */
    UG_SIGNAL_CLICKED = 200
};

/*: An UG event number.  */
typedef enum ug_event ug_event_t;

/*: An UG signal number.  Not to be confused with Unix signals.  */
typedef enum ug_signal ug_signal_t;

/*: An opaque data type, used when dealing with event
 *  (or signal) data. To retrieve information from this data type, you must
 *  use the correct ug_event_* functions.
 */
typedef void ug_event_data_t;

/*: Send an event (including event data) to a widget.  Events are
 *  messages handled by a widget's class.  */
void ug_widget_send_event (ug_widget_t *, ug_event_t, ug_event_data_t *);

/*: Emit a signal (including event data) to a widget.  Signals are
 *  like events, but are handled by user-defined slots, rather than
 *  the widget's class.  */
void ug_widget_emit_signal (ug_widget_t *, ug_event_t, ug_event_data_t *);

/*: Send an event to a widget, with keyboard event data.  */
void ug_widget_send_event_key (ug_widget_t *, ug_event_t, int key);

/*: Emit a signal to a widget, with keyboard event data.  */
void ug_widget_emit_signal_key (ug_widget_t *, ug_signal_t, int key);

/*: Return the key in a keyboard event.  */
int ug_event_data_key (ug_event_data_t *);

/*: Send an event to a widget, with mouse event data.  */
void ug_widget_send_event_mouse (ug_widget_t *, ug_event_t, int x, int y, int b, int bstate);

/*: Emit a signal to a widget, with mouse event data.  */
void ug_widget_emit_signal_mouse (ug_widget_t *, ug_signal_t, int x, int y, int b, int bstate);

/*: Return the x position of a mouse event.  */
int ug_event_data_mouse_x (ug_event_data_t *);

/*: Return the y position of a mouse event.  */
int ug_event_data_mouse_y (ug_event_data_t *);

/*: Return the relative x position of a mouse event.  */
int ug_event_data_mouse_rel_x (ug_event_data_t *);

/*: Return the relative y position of a mouse event.  */
int ug_event_data_mouse_rel_y (ug_event_data_t *);

/*: Return the mouse button of a mouse event.  */
int ug_event_data_mouse_b (ug_event_data_t *);

/*: Return the mouse button state of a mouse event.  */
int ug_event_data_mouse_bstate (ug_event_data_t *);

/*: Send an event to a widget, with drawing event data.  */
void ug_widget_send_event_draw (ug_widget_t *, ug_event_t, struct BITMAP *bmp);

/*: Emit a signal to a widget, with drawing event data.  */
void ug_widget_emit_signal_draw (ug_widget_t *, ug_signal_t, struct BITMAP *bmp);

/*: Return the bitmap address of a draw event.  */
struct BITMAP *ug_event_data_draw_bmp (ug_event_data_t *);

/*: Return the x position of a draw event.  */
int ug_event_data_draw_x (ug_event_data_t *);

/*: Return the y position of a draw event.  */
int ug_event_data_draw_y (ug_event_data_t *);

/*: Return the width of a draw event.  */
int ug_event_data_draw_w (ug_event_data_t *);

/*: Return the height of a draw event.  */
int ug_event_data_draw_h (ug_event_data_t *);


/* uglayout.c */

/* ug_dialog_layout_t:
 *  An array of this structure type tells UG about the layout of widgets
 *  that make up a dialog.  The structure contains the following fields:
 * 
 *  - CLASS: the widget class to use.
 * 
 *  - W: the width of the widget.  This may be negative, in which case
 *  	it will be interpreted as a percentage of the full dialog's width
 * 	(e.g. -50 means 50% of its parent).
 * 
 *  - H: the height of the widget.
 *
 *  - DATA: extra data for the widget class.  For example, buttons would
 *  	have a string here for the text label.
 * 
 *  - SLOT: a function with the prototype:
 *
 *	void slot (ug_widget_t *w, ug_signal_t sig, void *data);
 * 
 *  	The function will be called whenever the widget emits a
 *  	signal.  It may be NULL if unneeded.
 * 
 *  - ID: a string, which can be used to search through a dialog for a
 *  	particular widget.  It may be NULL if unneeded.
 * 
 *  The array must be terminated with a UG_DIALOG_LAYOUT_END sentinel,
 *  and where you want the layout engine to create a new row, you need to
 *  put a special marker, UG_DIALOG_LAYOUT_BR.
 * 
 *  Example:
 * 
 *    ug_dialog_layout_t my_dlg[] = {
 *        { ug_button, -100, 16, "button one", slot1, "b1" },
 *        { UG_DIALOG_LAYOUT_BR },
 *        { ug_button, -50, 16, "button two", NULL, NULL },
 *        { ug_button, -50, 16, "button three", slot3, "b3" },
 *        { UG_DIALOG_LAYOUT_END }
 *    };
 */
typedef struct ug_dialog_layout {
    ug_widget_class_t *class;
    int w, h;
    void *data;
    void (*slot) (ug_widget_t *, ug_signal_t, void *);
    const char *id;
} ug_dialog_layout_t;

#define UG_DIALOG_LAYOUT_BR	((ug_widget_class_t *)  0), 0, 0, 0, 0, 0
#define UG_DIALOG_LAYOUT_END	((ug_widget_class_t *) -1), 0, 0, 0, 0, 0

/*: Create a dynamic dialog layout and return a pointer to it.  If 
 *  possible, you should use a static dialog layout instead of this.  */
ug_dialog_layout_t *ug_dialog_layout_create ();

/*: Free the memory associated with a dynamic dialog layout.  */
void ug_dialog_layout_destroy (ug_dialog_layout_t *);

/*: Add a layout item onto the back of a dynamic dialog layout, and
 *  return the new address of the dialog.  */
ug_dialog_layout_t *ug_dialog_layout_insert (ug_dialog_layout_t *dialog,
					     ug_dialog_layout_t *newitem);


/* ugdialog.c */

/*: Opaque data type for holding dialogs.  */
typedef struct ug_dialog ug_dialog_t;

/*: Create a dialog with the specified layout in a window, and with a
 *  specified border.  */
ug_dialog_t *ug_dialog_create (struct gui_window *, ug_dialog_layout_t *, int border);

/*: Free memory associated with a dialog.  */
void ug_dialog_destroy (ug_dialog_t *);

/*: Mark the window that a dialog occupies for redrawing.  */
void ug_dialog_dirty (ug_dialog_t *);

/*: Give the focus to the specified widget in the dialog.
 *  See also: ug_widget_focus.  */
void ug_dialog_focus (ug_dialog_t *, ug_widget_t *);

/*: Return the x position of a dialog.  */
int ug_dialog_x (ug_dialog_t *);

/*: Return the y position of a dialog.  */
int ug_dialog_y (ug_dialog_t *);

/*: Return the width of a dialog.  */
int ug_dialog_w (ug_dialog_t *);

/*: Return the height of a dialog.  */
int ug_dialog_h (ug_dialog_t *);

/*: Return the widget in the dialog with the specified id. */
ug_widget_t *ug_dialog_widget (ug_dialog_t *, char *id);


/* ugblank.c */

/*: A dummy widget class that takes up space, but does nothing.  */
extern ug_widget_class_t ug_blank;


/* ugbutton.c */

/*: A widget class that implements a push button.  The `data' field is
 *  the text label on the button.  When clicked, it emits a
 *  UG_SIGNAL_CLICKED signal, with mouse event data.  */
extern ug_widget_class_t ug_button;

/*: Buttons have an `extra' field, which can point to anything you want.
 *  This sets the `extra' field of a button to EXTRA.  */
void ug_button_set_extra (ug_widget_t *, void *extra);

/*: Return the data stored in a ug_button widget's `extra' field.  */
void *ug_button_extra (ug_widget_t *);


/* ugmenu.c */

/*: A widget class implementing popup menus.  The `data' field points to
 *  a ug_menu_root_t structure.  When any one of the items in the menu is
 *  clicked, it emits a UG_SIGNAL_CLICKED signal.  The data parameter of
 *  the slot function will be passed the contents of the selected menu
 *  item's `data' field.  See also: ug_menu_item_t, ug_menu_root_t.  */
extern ug_widget_class_t ug_menu;

/* ug_menu_item_t:
 *  Holds infomation about a single menu item.  The two fields are:
 *  
 *  - LABEL: the label to use for the menu item
 *
 *  - DATA: arbitrary data to be associated with this menu item.  This 
 *	will be passed to the slot function if this item is selected.
 * 
 *  Example:
 *  
 *    ug_menu_item_t items[] = {
 *        { "Foo Item", foodata },
 *        { "Bar Item", bardata },
 *        { NULL, NULL }
 *    };
 */
typedef struct ug_menu_item {
    char *label;
    void *data;
} ug_menu_item_t;

/* ug_menu_root_t:
 *  Holds information about a menu.  It has two fields:
 *
 *  - LABEL: the label to use for the menu root button
 * 
 *  - ITEM: a pointer to an array of ug_menu_item_t's.  The array must be
 *  terminated with a ug_menu_item_t containing NULL for both the `label'
 *  and the `data' fields. 
 * 
 *  Example:
 *
 *    ug_menu_root_t the_menu = { "Quux label", items };
 */
typedef struct ug_menu_root {
    char *label;
    ug_menu_item_t *item;
} ug_menu_root_t;


#endif
