/* ugmenu.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "ug.h"
#include "uginter.h"
#include "ugtheme.h"
#include "ugbutton.h"


static void unpop (ug_widget_t *p);


struct menu {
    ug_menu_root_t *root;
    
    int down;
    int popped;
    gui_window_t *window;
    ug_dialog_t *dialog;
};

#define private(x)		((struct menu *) (x)->private)
#define last_item(root, i)	(!root->item[i].label)


static int menu_create (ug_widget_t *p, void *root)
{
    if (!root)
	return -1;
    
    p->private = malloc (sizeof (struct menu));
    if (!p->private)
	return -1;
    memset (p->private, 0, sizeof (struct menu));

    private (p)->root = root;
    return 0;
}


static void menu_destroy (ug_widget_t *p)
{
    free (private (p));
}


static void draw (ug_widget_t *p, BITMAP *bmp)
{
    int ofs = (private (p)->down) ? 1 : 0;

    ug_theme_tile (bmp, p->x + ofs, p->y + ofs, p->w - ofs, p->h - ofs, UG_THEME_FG);
    ug_theme_bevel (bmp, p->x, p->y, p->w, p->h, private (p)->down);

    if (private (p)->root->label)
	ug_theme_text_centre (bmp, p->x + ofs, p->y + ofs, p->w, p->h,
			      UG_THEME_FG, private (p)->root->label);
}


static void item_slot (ug_widget_t *p, int signal, void *d)
{
    ug_widget_t *parent;
    ug_menu_root_t *root;
    int i;
    
    if (signal != UG_SIGNAL_CLICKED)
	return;
    
    parent = ug_button_extra (p);
    root = private (parent)->root;

    for (i = 0; !last_item (root, i); i++)
	if (!ustrcmp (root->item[i].label, ug_widget_id (p)))
	    break;

    if (!last_item (root, i))
	ug_widget_emit_signal_ex (parent, UG_SIGNAL_CLICKED,
				  root->item[i].data);
	
    unpop (parent);
}


static int count_item (ug_menu_root_t *root)
{
    int i;
    for (i = 0; root->item[i].label; i++)
	;
    return i;
}


static int calc_width (ug_menu_root_t *root)
{
    int width = 0, i, w;
    
    for (i = 0; !last_item (root, i); i++) {
	w = text_length (ug_theme_font (UG_THEME_FG), root->item[i].label);
	width = MAX (w, width);
    }

    return width + 25;
}


static void pop (ug_widget_t *p)
{
    struct menu *m = private (p);
    ug_dialog_layout_t *lay;
    ug_widget_t *widget;
    int width, height, y, i;

    width = calc_width (m->root);
    height = count_item (m->root) * 16;

    if (ug_widget_y (p) + ug_widget_h (p) + height < SCREEN_H)
	y = ug_widget_y (p) + ug_widget_h (p);
    else
	y = ug_widget_y (p) - height;
    
    m->window = gui_window_create (ug_widget_x (p), y, width, height,
				   GUI_HINT_NOFRAME);
    gui_window_set_depth (m->window, 10);
    
    lay = ug_dialog_layout_create ();

    /* each item gets a button */
    for (i = 0; !last_item (m->root, i); i++) {
	ug_dialog_layout_t widget = { 
	    	&ug_button, width, 16,
		m->root->item[i].label, item_slot, 
		m->root->item[i].label
	};
	
	ug_dialog_layout_t br = { UG_DIALOG_LAYOUT_BR };
	
	lay = ug_dialog_layout_insert (lay, &widget);
	lay = ug_dialog_layout_insert (lay, &br);
    }

    /* attach it to the window */
    m->dialog = ug_dialog_create (m->window, lay, 0);

    /* each item's button points back to the root */
    for (i = 0; !last_item (m->root, i); i++) {
	widget = ug_dialog_widget (m->dialog, m->root->item[i].label);
	ug_button_set_extra (widget, p);
    }

    ug_dialog_layout_destroy (lay);

    private (p)->popped = 1;
}


static void unpop (ug_widget_t *p)
{
    ug_dialog_destroy (private (p)->dialog);
    gui_window_destroy (private (p)->window);
    private (p)->popped = 0;
}


static void menu_event (ug_widget_t *p, int event, void *d)
{
    switch (event) {
	case UG_EVENT_WIDGET_DRAW:
	    draw (p, ug_event_draw_bmp (d));
	    break;

	case UG_EVENT_MOUSE_DOWN:
	    if (ug_event_mouse_b (d) == 0) {
		private (p)->down = 1;
		ug_widget_dirty (p);
	    }
	    break;

	case UG_EVENT_MOUSE_UP:
	    if (ug_event_mouse_b (d) == 0) {
		if (!private (p)->popped)
		    pop (p);
		else
		    unpop (p);

		private (p)->down = 0;
		ug_widget_dirty (p);
	    }
	    break;
    }
}


ug_widget_class_t ug_menu = {
    menu_create,
    menu_destroy,
    menu_event
};
