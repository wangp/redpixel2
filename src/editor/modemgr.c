/* modemgr.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <stdlib.h>
#include "gui.h"
#include "ug.h"
#include "modemgr.h"


static void modebar_make (void);


static struct editmode **modes;
static int num, current;


void modemgr_init (void)
{
    modes = 0;
    num = 0;
    current = -1;
}

void modemgr_shutdown (void)
{
    free (modes);
}

void modemgr_register (struct editmode *mode)
{
    modes = realloc (modes, sizeof (struct editmode *) * (num + 1));
    modes[num] = mode;
    num++;
    
    modebar_make ();
}

void modemgr_select (char *name)
{
    int i;

    for (i = 0; i < num; i++)
	if (!strcmp (name, modes[i]->name)) {
	    if (current >= 0)
		modes[current]->leave_mode ();
	    modes[i]->enter_mode ();
	    current = i;
	    return;
	}
}


/*----------------------------------------------------------------------*/

/*
 * modebar: visible side of the mode manager.  Probably should be in a
 * separate file, but this way is easier and less code duplication.
 */


static void slot (ug_widget_t *p, ug_signal_t signal, void *d);


static gui_window_t *window;
static ug_dialog_t *dialog;


void modebar_install (int x, int y, int w, int h)
{
    window = gui_window_create (x, y, w, h, GUI_HINT_NOFRAME | GUI_HINT_STEALFOCUS);
    gui_window_set_depth (window, -1);

    /* attached in make function */
    dialog = 0;
}


void modebar_uninstall (void)
{
    ug_dialog_destroy (dialog);
    gui_window_destroy (window);
}


static void modebar_make (void)
{
    ug_dialog_layout_t *lay;
    int i;

    lay = ug_dialog_layout_create ();

    /* force items to "right align" */ {
	ug_dialog_layout_t filler = { &ug_blank, -100, -100, 0, 0, 0 };
	lay = ug_dialog_layout_insert (lay, &filler);
    }

    /* each mode gets a button */
    for (i = 0; i < num; i++) {
	ug_dialog_layout_t widget = { &ug_button, 48, -100, modes[i]->name, slot, modes[i]->name };
	lay = ug_dialog_layout_insert (lay, &widget);
    }

    /* attach it to the window */
    if (dialog) ug_dialog_destroy (dialog);
    dialog = ug_dialog_create (window, lay, 0);

    ug_dialog_layout_destroy (lay);
}


static void slot (ug_widget_t *p, ug_signal_t signal, void *d)
{
    char *id = ug_widget_id (p);
    int i;

    if (signal == UG_SIGNAL_CLICKED)
	for (i = 0; i < num; i++)
	    if (!strcmp (modes[i]->name, id)) {
		if (num != current)
		    modemgr_select (id);
		break;
	    }
}
