#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "fe-widgets.h"
#include "strlcpy.h"
#include "yield.h"

static int gray, light_gray, white;
DIALOG_PLAYER *fancy_active_player;
BITMAP *fancy_screen;
FONT *fancy_edit_font;
FONT *fancy_font;
SAMPLE *type_sound;


typedef struct
{
    int clip;
    int cl, cr, ct, cb;
} clip_t;


static void save_clip (BITMAP *bmp, clip_t *clip)
{
    clip->clip = bmp->clip;
    clip->cl = bmp->cl;
    clip->cr = bmp->cr;
    clip->ct = bmp->ct;
    clip->cb = bmp->cb;
}


static void restore_clip (BITMAP *bmp, clip_t *clip)
{
    bmp->clip = clip->clip;
    bmp->cl = clip->cl;
    bmp->cr = clip->cr;
    bmp->ct = clip->ct;
    bmp->cb = clip->cb;
}


static void highlight_widget (DIALOG *d)
{
    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
    set_add_blender (0xff, 0xff, 0xff, 0x40);
    rect_wh (fancy_screen, d->x + 1, d->y + 1, d->w - 2, d->h - 2, white);
    rect_wh (fancy_screen, d->x + 2, d->y + 2, d->w - 4, d->h - 4, white);
    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
}


/*------------------------------------------------------------*/
/* The widgets                                                */
/*------------------------------------------------------------*/

/* fancy_bitmap_proc blits to fancy_screen, otherwise it is exactly
   the same as d_bitmap_proc. */
int fancy_bitmap_proc (int msg, DIALOG *d, int c)
{
    if ((msg == MSG_DRAW) && (d->dp)) {
	blit (d->dp, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);
	d->flags |= D_FANCY_DIRTY;
    }

    return D_O_K;
}


/* fancy_label_proc writes dp onto the fancy_screen with transparency. d1
   stores alignment, d2 the alpha.  dp2 can point to a custom font. */
int fancy_label_proc (int msg, DIALOG *d, int c)
{
    if (msg == MSG_DRAW) {
	FONT *fnt = d->dp2 ? d->dp2 : font;
	int rtm = text_mode (d->bg);
	int y = (d->h - text_height (fnt)) / 2;
	BITMAP *text_bmp = create_bitmap (d->w, d->h);
	clear_to_color (text_bmp, bitmap_mask_color (text_bmp));

	if (d->d1 == 1)
	    textout_centre (text_bmp, fnt, d->dp, d->w / 2, y, d->fg);
	else if (d->d1 == 2)
	    textout_right (text_bmp, fnt, d->dp, d->w, y, d->fg);
	else
	    textout (text_bmp, fnt, d->dp, 0, y, d->fg);

	set_trans_blender (0xff, 0xff, 0xff, d->d2);
	draw_trans_sprite (fancy_screen, text_bmp, d->x, d->y);

	destroy_bitmap (text_bmp);
	text_mode (rtm);

	d->flags |= D_FANCY_DIRTY;
    }

    return D_O_K;
}


/* fancy_button_proc blits dp2 onto the fancy_screen, and calls function dp3
   when pushed.  dp test, d2 alpha of text. */
int fancy_button_proc (int msg, DIALOG *d, int c)
{
    switch (msg) {
	case MSG_DRAW:
	    if (d->dp2)
		blit (d->dp2, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    /* Hover. */
	    if (d->flags & D_GOTFOCUS)
		highlight_widget (d);

	    /* Mouse held. */
	    if (d->flags & D_SELECTED) {
		drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
		set_add_blender (0xff, 0xff, 0xff, 0x40);
		rect_wh (fancy_screen, d->x + 1, d->y + 1, d->w - 2, d->h - 2, gray);
		rect_wh (fancy_screen, d->x + 2, d->y + 2, d->w - 4, d->h - 4, makecol(0x80, 0x80, 0xff));
		rect_wh (fancy_screen, d->x + 3, d->y + 3, d->w - 6, d->h - 6, gray);
		drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
	    }

	    d->flags |= D_FANCY_DIRTY;
	    if (gui_mouse_b ())
		blit_fancy_dirty_to_screen (active_dialog);
	    break;

	case MSG_CLICK: {
	    int state1 = 0, state2 = 0;

	    while (gui_mouse_b()) {
		state2 = ((gui_mouse_x() >= d->x) && (gui_mouse_x() < d->x + d->w) &&
			  (gui_mouse_y() >= d->y) && (gui_mouse_y() < d->y + d->h));

		/* redraw? */
		if (state2 != state1) {
		    if (state2) d->flags |= D_SELECTED;
		    else d->flags &=~ D_SELECTED;
		    state1 = state2;
		    d->flags |= D_DIRTY;
		}

		/* let other objects continue to animate */
		broadcast_dialog_message (MSG_IDLE, 0);
	    }

	    if (d->flags & D_SELECTED) {
		d->flags &=~ D_SELECTED;
		d->flags |= D_DIRTY;

		if (d->dp3) {
		    int (*proc)(void) = d->dp3;
		    return proc ();
		}

		if (d->flags & D_EXIT)
		    return D_CLOSE;
	    }
	    break;
	}

	case MSG_WANTFOCUS:
	    return D_WANTFOCUS;

	case MSG_KEY:
	    if (d->dp3) {
		int (*proc)(void) = d->dp3;
		return proc ();
	    }

	    if (d->flags & D_EXIT) return D_CLOSE;
	    break;
    }

    return D_O_K;
}


/* fancy_checkbox_proc blits dp stores the test, dp2 bitmap, dp3
   optional tick bitmap. d2 stores alpha of the text.  It is ticked if
   marked D_SELECTED. */
int fancy_checkbox_proc (int msg, DIALOG *d, int c)
{
    switch (msg) {
        case MSG_DRAW:
	    if (d->dp2)
		blit (d->dp2, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    /* Hover. */
	    if (d->flags & D_GOTFOCUS) {
		drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
		set_add_blender (0xff, 0xff, 0xff, 0x40);
		rect_wh (fancy_screen, d->x + 6, d->y + 6, d->h - 12, d->h - 12, white);
		rect_wh (fancy_screen, d->x + 7, d->y + 7, d->h - 14, d->h - 14, white);
		drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
	    }

	    /* Toggled. */
	    if ((d->flags & D_SELECTED) && d->dp3) {
		set_alpha_blender ();
		draw_trans_sprite (fancy_screen, d->dp3, d->x + 5, d->y + 5);
	    }

	    d->flags |= D_FANCY_DIRTY;
	    if (gui_mouse_b ())
		blit_fancy_dirty_to_screen (active_dialog);
	    break;

	case MSG_CLICK:
	    return d_button_proc (msg, d, c);

        case MSG_WANTFOCUS:
	    return D_WANTFOCUS;
    }

    return D_O_K;
}


static void rotated_rect (BITMAP *bmp, int x, int y, int h, int border, int colour)
{
    /*    x1 x2 x3
     * y1    .
     *      / \
     * y2  .   .
     *      \ /
     * y3    .
     */

    int x1 = x + 5 + border;
    int x2 = x + 5 + h / 2;
    int x3 = x + 5 - border + h;
    int y1 = y + 5 + border;
    int y2 = y + 5 + h / 2;
    int y3 = y + 5 - border + h;

    line (bmp, x1, y2, x2 - 1, y1 + 1, colour);
    line (bmp, x2, y1, x3 - 1, y2 - 1, colour);
    line (bmp, x3, y2, x2 + 1, y3 - 1, colour);
    line (bmp, x2, y3, x1 + 1, y2 + 1, colour);
}

/* d1 group number, d2 alpha, dp text, dp2 bitmap, dp3 an optional callback func. */
int fancy_radio_proc (int msg, DIALOG *d, int c)
{
    if (msg == MSG_DRAW) {
	if (d->dp2)
	    blit (d->dp2, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	if (d->flags & D_DISABLED) {
	    int rtm = text_mode (d->bg);
	    textout (fancy_screen, fancy_font, d->dp, d->x + d->h, d->y + (d->h - text_height (fancy_font)) / 2, gray);
	    text_mode (rtm);
	}

	/* Hover. */
	if (d->flags & D_GOTFOCUS) {
	    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	    set_add_blender (0xff, 0xff, 0xff, 0x30);
	    rotated_rect (fancy_screen, d->x, d->y, d->h - 10, 1, white);
	    rotated_rect (fancy_screen, d->x, d->y, d->h - 10, 2, white);
	    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
	}

	/* Selected. */
	if (d->flags & D_SELECTED) {
	    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	    set_add_blender (0xff, 0xff, 0xff, 0x80);
	    rotated_rect (fancy_screen, d->x, d->y, d->h - 10, 1, gray);
	    rotated_rect (fancy_screen, d->x, d->y, d->h - 10, 2, makecol (0x80, 0x80, 0xff));
	    rotated_rect (fancy_screen, d->x, d->y, d->h - 10, 3, gray);
	    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
	}

	d->flags |= D_FANCY_DIRTY;
	if (gui_mouse_b ())
	    blit_fancy_dirty_to_screen (active_dialog);
	return D_O_K;
    }
    else if ((msg == MSG_CLICK) && (d->dp3) && !(d->flags & D_SELECTED)) {
	int state1 = 0, state2;
	void (*proc)(void) = d->dp3;

	while (gui_mouse_b ()) {
	    state2 = ((gui_mouse_x() >= d->x) && (gui_mouse_x() < d->x + d->w) &&
		      (gui_mouse_y() >= d->y) && (gui_mouse_y() < d->y + d->h));

	    /* redraw? */
	    if (state2 != state1) {
		if (state2) d->flags |= D_SELECTED;
		else d->flags &=~ D_SELECTED;
		state1 = state2;
		d->flags |= D_DIRTY;
	    }

	    /* let other objects continue to animate */
	    broadcast_dialog_message (MSG_IDLE, 0);
	}

	if (d->flags & D_SELECTED) {
	    broadcast_dialog_message (MSG_RADIO, d->d1);
	    d->flags |= D_SELECTED;
	    proc ();
	}

	return D_REDRAWME;
    }

    return d_radio_proc (msg, d, c);
}

/* fancy_edit_proc blits dp2 onto the fancy_screen, and calls function dp3
   when enter is pressed.  */
int fancy_edit_proc (int msg, DIALOG *d, int c)
{
    switch (msg) {
	case MSG_DRAW: {
	    int h = text_height (fancy_edit_font);
	    int y = d->y + (d->h - h) / 2;
	    int l;
	    int rtm = text_mode (d->bg);
	    char buf[256];
	    clip_t old_clip;

	    if (d->dp2)
		blit (d->dp2, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    /* Hover. */
	    if (d->flags & D_GOTFOCUS)
		highlight_widget (d);

	    /* Get how far along the cursor is, in pixels. */
	    strlcpy (buf, (char *)d->dp, d->d2 + 1);
	    l = text_length (fancy_edit_font, buf);

	    save_clip (fancy_screen, &old_clip);
	    set_clip_wh (fancy_screen, d->x + 5, d->y, d->w - 11, d->h);

	    if (l < d->w - 12) {
		if (d->flags & D_GOTFOCUS) {
		    vline (fancy_screen, d->x + l + 5, y - 5, y + h, light_gray);
		    vline (fancy_screen, d->x + l + 6, y - 5, y + h, light_gray);
		}
		textout (fancy_screen, fancy_edit_font, d->dp,
			 d->x + 5, y, d->fg);
	    }
	    else {
		if (d->flags & D_GOTFOCUS) {
		    vline (fancy_screen, d->x + d->w - 7, y - 5, y + h, light_gray);
		    vline (fancy_screen, d->x + d->w - 8, y - 5, y + h, light_gray);
		}
		textout (fancy_screen, fancy_edit_font, d->dp,
			 d->x + d->w - 5 - l, y, d->fg);
	    }
	    restore_clip (fancy_screen, &old_clip);

	    text_mode (rtm);

	    d->flags |= D_FANCY_DIRTY;
	    return D_O_K;
	}

	case MSG_CHAR:
	    if (c >> 8 == KEY_ENTER) {
		if (d->dp3) {
		    int (*proc)(void) = d->dp3;
		    return proc ();
		}
	    }

	    if (key_shifts & KB_CTRL_FLAG) {
		if (c >> 8 == KEY_A)
		    d->d2 = 0;
		else if (c >> 8 == KEY_B)
		    d->d2 = MAX (0, d->d2 - 1);
		else if (c >> 8 == KEY_E)
		    d->d2 = strlen (d->dp);
		else if (c >> 8 == KEY_F)
		    d->d2 = MIN (d->d2 + 1, (int) strlen (d->dp));
		else if (c >> 8 == KEY_K)
		    ((char *)d->dp)[d->d2] = '\0';
		else if (c >> 8 == KEY_U)
		    ((char *)d->dp)[0] = '\0';

		d->flags |= D_DIRTY;
		return D_USED_CHAR;
	    }

	    /* Don't want some keys to use the typing sound. */
	    if ((c >> 8 != KEY_UP) && (c >> 8 != KEY_DOWN) &&
		(c >> 8 != KEY_LEFT) &&	(c >> 8 != KEY_RIGHT) &&
		(c >> 8 != KEY_TAB) && (c >> 8 != KEY_ESC))
		play_sample (type_sound, 255, 128, 1000, FALSE);
	    break;
    }

    return d_edit_proc (msg, d, c);
}


/* fancy_list_proc blits dp3 onto the screen as a background for the
   widget.  dp1 is a function which feeds it text, like d_list_proc.
   no focus, no scrollbar. */
int _fancy_list_proc (int msg, DIALOG *d, int c, FONT *fnt)
{
    char *(*proc)(int, int *) = d->dp;
    int height, listsize, ret = D_O_K;
    FONT *orig_font = font;

    if (!proc)
	return D_O_K;

    font = fnt;
    height = (d->h - 4) / text_height (font); /* Number of rows of text. */

    switch (msg) {
	case MSG_DRAW: {
	    char str[1024];
	    int i;
	    int rtm = text_mode (d->bg);
	    clip_t old_clip;

	    proc (-1, &listsize);

	    /* Background. */
	    if (d->dp3)
		blit (d->dp3, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    /* The text. */
	    save_clip (fancy_screen, &old_clip);
	    set_clip_wh (fancy_screen, d->x + 5, d->y, d->w - 11, d->h);

	    for (i = 0; (i < height) && (i + d->d2 < listsize); i++) {
		ustrzcpy (str, sizeof str, proc (i + d->d2, NULL));
		if (str)
		    textout (fancy_screen, font, str, d->x + 5,
			     d->y + 2 + i * text_height (font), d->fg);
	    }

	    restore_clip (fancy_screen, &old_clip);

	    text_mode (rtm);

	    d->flags |= D_FANCY_DIRTY;
	    ret = D_O_K;
	    break;
	}

        case MSG_CHAR: {
	    int old_d2 = d->d2;

	    proc (-1, &listsize);

	    if (c >> 8 == KEY_UP)
		d->d2--;
	    else if (c >> 8 == KEY_DOWN)
		d->d2++;
	    else if (c >> 8 == KEY_PGUP)
		d->d2 -= height - 1;
	    else if (c >> 8 == KEY_PGDN)
		d->d2 += height - 1;
	    else if (c >> 8 == KEY_HOME)
		d->d2 = 0;
	    else if (c >> 8 == KEY_END)
		d->d2 = listsize - height;
	    else {
		ret = d_list_proc (msg, d, c);
		break;
	    }

	    d->d2 = MID (0, d->d2, listsize - height);

	    if (d->d2 != old_d2)
		d->flags |= D_DIRTY;
	    ret = D_USED_CHAR;
	    break;
	}

        case MSG_WANTFOCUS:
	    ret = D_WANTFOCUS;
	    break;
    }

    font = orig_font;
    return ret;
}


/* d1 max, d2 slider pos, dp2 callback function, dp3 bitmap. */
int fancy_slider_proc (int msg, DIALOG *d, int c)
{
    int old_d2 = d->d2;
    int step_size = (d->d1 > 20) ? d->d1 / 20 : 1;
    void (*proc)(DIALOG *) = d->dp2;

    switch (msg) {
        case MSG_DRAW: {
 	    int w = d->w - 20;
	    int x = d->x + 5 + (w * d->d2 / d->d1);

	    if (d->dp3)
		blit (d->dp3, fancy_screen, 0, 0, d->x, d->y, d->w, d->h);

	    /* Hover. */
	    if (d->flags & D_GOTFOCUS)
		highlight_widget (d);

	    /* Slider handle.*/
	    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
	    set_add_blender (0xff, 0xff, 0xff, 0x40);
	    rectfill_wh (fancy_screen, x, d->y + 5, 10, d->h - 10, gray);
	    rect_wh (fancy_screen, x, d->y + 5, 10, d->h - 10, white);

	    set_burn_blender (0xff, 0xff, 0xff, 0x40);
	    vline (fancy_screen, x + 4, d->y + 10, d->y + d->h - 10, gray);
	    vline (fancy_screen, x + 5, d->y + 10, d->y + d->h - 10, white);
	    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);

	    d->flags |= D_FANCY_DIRTY;
	    if (gui_mouse_b ())
		blit_fancy_dirty_to_screen (active_dialog);
	    break;
	}

	case MSG_WANTFOCUS:
	    return D_WANTFOCUS;

        case MSG_CLICK:
	    while (gui_mouse_b()) {
		int mx = gui_mouse_x ();

		if ((d->w - 20) / d->d1 > 1)
		    mx += ((d->w - 20) / d->d1) / 2;

		d->d2 = (mx - d->x - 10) * d->d1 / (d->w - 20);
		d->d2 = MID (0, d->d2, d->d1);

		/* redraw? */
		if (d->d2 != old_d2) {
		    d->flags |= D_DIRTY;
		    old_d2 = d->d2;

		    /* callback */
		    if (proc)
			proc (d);
		}

		/* let other objects continue to animate */
		broadcast_dialog_message (MSG_IDLE, 0);
	    }
	    break;

        case MSG_WHEEL:
	    /* Wheel down slides to the right. */
	    /* d->d2 -= c * step_size; */

	    /* Wheel up slides to the right. */
	    d->d2 += c * step_size;

	    d->d2 = MID (0, d->d2, d->d1);

	    if (d->d2 != old_d2) {
		d->flags |= D_DIRTY;

		/* callback */
		if (proc)
		    proc (d);
	    }
	    break;

        case MSG_CHAR:
	    if (c >> 8 == KEY_LEFT)
		d->d2 -= step_size;
	    else if (c >> 8 == KEY_RIGHT)
		d->d2 += step_size;
	    else
		break;

	    d->d2 = MID (0, d->d2, d->d1);

	    if (d->d2 != old_d2) {
		d->flags |= D_DIRTY;

		/* callback */
		if (proc)
		    proc (d);
	    }

	    return D_USED_CHAR;
    }

    return D_O_K;
}


/*------------------------------------------------------------*/

static void draw_trans_text (BITMAP *bmp, const char *str, int align, int x, int fg, int bg, int alpha)
{
    BITMAP *text_bmp = create_bitmap (bmp->w, bmp->h);
    int rtm = text_mode (bg);
    int y = (bmp->h - text_height (fancy_font)) / 2;

    clear_to_color (text_bmp, bitmap_mask_color (text_bmp));

    if (align == 0)
	textout (text_bmp, fancy_font, str, x, y, fg);
    else if (align == 1)
	textout_centre (text_bmp, fancy_font, str, bmp->w / 2 + x, y, fg);

    set_add_blender (0xff, 0xff, 0xff, alpha);
    draw_trans_sprite (bmp, text_bmp, 0, 0);

    destroy_bitmap (text_bmp);
    text_mode (rtm);
}


void make_button_background (DIALOG *d, BITMAP *bmp, BITMAP *backdrop, int bg)
{
    if (backdrop)
        blit (backdrop, bmp, d->x, d->y, 0, 0, d->w, d->h);

    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
    set_burn_blender (0xff, 0xff, 0xff, 0x40);
    rectfill_wh (bmp, 0, 0, d->w, d->h, bg);

    set_add_blender (0xff, 0xff, 0xff, 0x30);
    rect_wh (bmp, 0, 0, d->w, d->h, white);
    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);
}


void set_fancy_label (DIALOG *d, int fg, FONT *font)
{
    d->fg = fg;
    if (font)
	d->dp2 = font;
}


void create_fancy_button (DIALOG *d, BITMAP *backdrop, int fg, int bg)
{
    d->fg = fg;

    /* The button. */
    d->dp2 = create_bitmap (d->w, d->h);
    make_button_background (d, d->dp2, backdrop, bg);

    /* The text. */
    if (d->dp)
	draw_trans_text (d->dp2, d->dp, 1, 0, d->fg, d->bg, d->d2);
}


void destroy_fancy_button (DIALOG *d)
{
    if (d->dp2)
        destroy_bitmap (d->dp2);
}


void create_fancy_checkbox (DIALOG *d, BITMAP *backdrop, int fg, int bg, BITMAP *tick)
{
    d->fg = fg;
    d->dp2 = create_bitmap (d->w, d->h);

    if (backdrop)
        blit (backdrop, d->dp2, d->x, d->y, 0, 0, d->w, d->h);

    if (tick)
	d->dp3 = tick;

    /* The tick box. */
    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
    set_burn_blender (0xff, 0xff, 0xff, 0x40);
    rectfill_wh (d->dp2, 5, 5, d->h - 10, d->h - 10, bg);

    set_add_blender (0xff, 0xff, 0xff, 0x30);
    rect_wh (d->dp2, 5, 5, d->h - 10, d->h - 10, white);
    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);

    /* The text. */
    if (d->dp)
	draw_trans_text (d->dp2, d->dp, 0, d->h, d->fg, d->bg, d->d2);
}

void destroy_fancy_checkbox (DIALOG *d)
{
    if (d->dp2)
	destroy_bitmap (d->dp2);
}

void create_fancy_radio_button (DIALOG *d, BITMAP *backdrop, int fg, int bg)
{
    int i, j;

    d->fg = fg;
    d->dp2 = create_bitmap (d->w, d->h);

    drawing_mode (DRAW_MODE_TRANS, NULL, 0, 0);
    set_burn_blender (0xff, 0xff, 0xff, 0x40);

    if (backdrop)
        blit (backdrop, d->dp2, d->x, d->y, 0, 0, d->w, d->h);
    rectfill_wh (d->dp2, 0, 0, d->w, d->h, bg);

    /* The radio box. */
    for (i = 0; i < d->h - 10; i++) {
	j = (i <= (d->h - 10) / 2) ? i : (d->h - 10 - i);
	hline (d->dp2, 5 + (d->h - 10) / 2 - j, 5 + i, 5 + (d->h - 10) / 2 + j, bg);
    }

    set_add_blender (0xff, 0xff, 0xff, 0x30);
    rotated_rect (d->dp2, 0, 0, d->h - 10, 0, white);
    drawing_mode (DRAW_MODE_SOLID, NULL, 0, 0);

    /* The text. */
    if (d->dp)
	draw_trans_text (d->dp2, d->dp, 0, d->h, d->fg, d->bg, d->d2);
}

void destroy_fancy_radio_button (DIALOG *d)
{
    if (d->dp2)
        destroy_bitmap (d->dp2);
}

void create_fancy_editbox (DIALOG *d, BITMAP *backdrop, int fg, int bg)
{
    d->fg = fg;
    d->dp2 = create_bitmap (d->w, d->h);
    make_button_background (d, d->dp2, backdrop, bg);
}


void destroy_fancy_editbox (DIALOG *d)
{
    if (d->dp2)
        destroy_bitmap (d->dp2);
}


void create_fancy_listbox (DIALOG *d, BITMAP *backdrop, int fg, int bg)
{
    d->fg = fg;
    d->dp3 = create_bitmap (d->w, d->h);
    make_button_background (d, d->dp3, backdrop, bg);
}


void destroy_fancy_listbox (DIALOG *d)
{
    if (d->dp3)
        destroy_bitmap (d->dp3);
}


void create_fancy_slider (DIALOG *d, BITMAP *backdrop, int fg, int bg)
{
    d->fg = fg;
    d->dp3 = create_bitmap (d->w, d->h);
    make_button_background (d, d->dp3, backdrop, bg);
}


void destroy_fancy_slider (DIALOG *d)
{
    if (d->dp3)
	destroy_bitmap (d->dp3);
}


/*------------------------------------------------------------*/

int fancy_gui_init (void)
{
    fancy_screen = create_bitmap (SCREEN_W, SCREEN_H);

    gray = makecol (0x80, 0x80, 0x80);
    light_gray = makecol (0xa0, 0xa0, 0xa0);
    white = makecol (0xff, 0xff, 0xff);

    return 0;
}


void fancy_gui_shutdown (void)
{
    destroy_bitmap (fancy_screen);
}


/*------------------------------------------------------------*/

int blit_fancy_dirty_to_screen (DIALOG *dialog)
{
    int blitted = 0;
    int i;
    DIALOG *d;

    for (i = 0; dialog[i].proc; i++) {
	d = dialog + i;

	if (d->flags & D_FANCY_DIRTY) {
	    d->flags &= ~D_FANCY_DIRTY;
	    scare_mouse_area (d->x, d->y, d->w, d->h);
	    blit (fancy_screen, screen, d->x, d->y, d->x, d->y, d->w, d->h);
	    unscare_mouse ();
	    blitted = 1;
	}
    }

    return blitted;
}


int fancy_do_dialog (DIALOG *dialog, int focus_obj)
{
    DIALOG_PLAYER *player;
    DIALOG_PLAYER *old_player;
    int ret;

    show_mouse (screen);

    player = init_dialog (dialog, focus_obj);

    old_player = fancy_active_player;
    fancy_active_player = player;
    
    while (update_dialog (player))
	blit_fancy_dirty_to_screen (dialog);

    ret = shutdown_dialog (player);

    fancy_active_player = old_player;

    return ret;
}
