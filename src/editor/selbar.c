/* selbar.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "ug.h"
#include "edselect.h"
#include "selbar.h"


static gui_window_t *window;
static ug_dialog_t *dialog;
static ug_widget_t *sel;
static ed_select_list_t *list;

static void (*left_proc) ();
static void (*right_proc) ();
static void (*selected_proc) ();


static void up_slot (ug_widget_t *p, int signal, void *data)
{
    ed_select_scroll_page_up (sel);
}

static void down_slot (ug_widget_t *p, int signal, void *data)
{
    ed_select_scroll_page_down (sel);
}

static void left_slot (ug_widget_t *p, int signal, void *data)
{
    if (left_proc) left_proc ();
}

static void right_slot (ug_widget_t *p, int signal, void *data)
{
    if (right_proc) right_proc ();
}

static void select_slot (ug_widget_t *p, int signal, void *data)
{
    if (selected_proc) selected_proc ();
}    

static ug_dialog_layout_t layout[] = {
    { &ug_button, -50,    14, "<",    left_slot,  0 },
    { &ug_button, -50,    14, ">",    right_slot, 0 },		{ UG_DIALOG_LAYOUT_BR },
    { &ug_button, -100,   13, "Up",   up_slot,    0 },		{ UG_DIALOG_LAYOUT_BR },
    { &ug_button, -100,   13, "Down", down_slot,  0 },	 	{ UG_DIALOG_LAYOUT_BR },
    { &ed_select, -100, -100, 0,      select_slot, "sel" },	{ UG_DIALOG_LAYOUT_END }
};


void selectbar_install (int x, int y, int w, int h)
{
    window = gui_window_create (x, y, w, h, GUI_HINT_NOFRAME | GUI_HINT_STEALFOCUS);
    gui_window_set_depth (window, -1);
    dialog = ug_dialog_create (window, layout, 0);
    sel = ug_dialog_widget (dialog, "sel");

    left_proc = right_proc = 0;
}

void selectbar_uninstall ()
{
    ug_dialog_destroy (dialog);
    gui_window_destroy (window);
}

void selectbar_set_list (ed_select_list_t *newlist)
{
    list = newlist;
    ed_select_set_list (sel, newlist);
}

void selectbar_set_icon_size (int w, int h)
{
    ed_select_set_icon_size (sel, w, h);
}

void selectbar_set_change_set_proc (void (*_left) (), void (*_right) ())
{
    left_proc = _left;
    right_proc = _right;
}

void selectbar_set_selected_proc (void (*proc) ())
{
    selected_proc = proc;
}

void selectbar_set_top (int top)
{
    ed_select_set_top (sel, top);
}

int selectbar_top ()
{
    return ed_select_top (sel);
}

void selectbar_set_selected (int selected)
{
    ed_select_set_selected (sel, selected);
}

int selectbar_selected ()
{
    return ed_select_selected (sel);
}

char *selectbar_selected_name ()
{
    return ed_select_list_item_name (list, selectbar_selected ());
}
