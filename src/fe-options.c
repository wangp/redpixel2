#include <string.h>
#include <allegro.h>
#include "error.h"
#include "fe-lobby.h"
#include "fe-main.h"
#include "fe-widgets.h"
#include "gamma.h"
#include "music.h"
#include "screen.h"
#include "set-video.h"
#include "sound.h"
#include "store.h"


static float old_music_desired_volume;
static int old_desired_brightness;
static int stretch_method_in_use;
static int old_desired_game_screen_w, old_desired_game_screen_h;

static DIALOG options_menu[];
#define DEFAULT_FOCUS			-1
#define BACKDROP			(options_menu[0])
#define SCREEN_RESOLUTION_BACKDROP	(options_menu[1])
#define SCREEN_RESOLUTION_LABEL		(options_menu[2])
#define SCREEN_RESOLUTION_RADIO_GROUP	1
#define RESOLUTION_320x200_RADIOBUTTON	(options_menu[3])
#define RESOLUTION_640x400_RADIOBUTTON	(options_menu[4])
#define RESOLUTION_640x480_RADIOBUTTON	(options_menu[5])
#define STRETCH_METHOD_BACKDROP		(options_menu[6])
#define STRETCH_METHOD_LABEL		(options_menu[7])
#define STRETCH_METHOD_RADIO_GROUP	2
#define NO_STRETCH_RADIOBUTTON		(options_menu[8])
#define PLAIN_STRETCH_RADIOBUTTON	(options_menu[9])
#define SUPER_2XSAI_RADIOBUTTON		(options_menu[10])
#define SUPER_EAGLE_RADIOBUTTON		(options_menu[11])
#define SFX_CHECKBOX			(options_menu[12])
#define SFX_SLIDER			(options_menu[13])
#define TEST_SFX_BUTTON			(options_menu[14])
#define MUSIC_CHECKBOX			(options_menu[15])
#define MUSIC_SLIDER			(options_menu[16])
#define GAMMA_LABEL			(options_menu[17])
#define GAMMA_SLIDER			(options_menu[18])
#define GAMMA_PREVIEW			(options_menu[19])
#define OK_BUTTON			(options_menu[20])
#define CANCEL_BUTTON			(options_menu[21])


static float gamma_slider_d2_to_gamma_factor (int d2)
{
    /*
     * d2 =  0/10 --> gamma = 1.5
     * d2 =  5/10 --> gamma = 1.0
     * d2 = 10/10 --> gamma = 0.5
     */
    return 0.5 + ((10 - d2) / 10.);
}


static int gamma_factor_to_gamma_slider_d2 (float gamma)
{
    /*
     * gamma = 1.5 --> d2 =  0/10
     * gamma = 1.0 --> d2 =  5/10
     * gamma = 0.5 --> d2 = 10/10
     */
    /* The 0.1 is there to avoid floating point inaccuracies that end
     * up being rounded downwards, e.g. 1.99999 -> 1.
     */
    return (10. - ((gamma_factor - 0.5) * 10.)) + 0.1;
}


static int radiobuttons_to_stretch_method (void)
{
    if (PLAIN_STRETCH_RADIOBUTTON.flags & D_SELECTED)
	return STRETCH_METHOD_PLAIN;
    else if (SUPER_2XSAI_RADIOBUTTON.flags & D_SELECTED)
	return STRETCH_METHOD_SUPER2XSAI;
    else if (SUPER_EAGLE_RADIOBUTTON.flags & D_SELECTED)
	return STRETCH_METHOD_SUPEREAGLE;
    else
	return STRETCH_METHOD_NONE;
}


static void stretch_method_to_radiobuttons (int method)
{
    broadcast_dialog_message (MSG_RADIO, STRETCH_METHOD_RADIO_GROUP);

    if (method == STRETCH_METHOD_PLAIN)
	PLAIN_STRETCH_RADIOBUTTON.flags |= D_SELECTED;
    else if (method == STRETCH_METHOD_SUPER2XSAI)
	SUPER_2XSAI_RADIOBUTTON.flags |= D_SELECTED;
    else if (method == STRETCH_METHOD_SUPEREAGLE)
	SUPER_EAGLE_RADIOBUTTON.flags |= D_SELECTED;
    else
	NO_STRETCH_RADIOBUTTON.flags |= D_SELECTED;
}


static int video_mode_is_big (int w, int h)
{
    /* The menu should try to be in the same resolution as the game to
       minimize the screen changing between the battle field and the
       lobby. */
    return ((w >= 640) && (h >= 400));
}


static void redraw_stretching_mode_radiobuttons (void)
{
    object_message (&NO_STRETCH_RADIOBUTTON, MSG_DRAW, 0);
    object_message (&PLAIN_STRETCH_RADIOBUTTON, MSG_DRAW, 0);
    object_message (&SUPER_2XSAI_RADIOBUTTON, MSG_DRAW, 0);
    object_message (&SUPER_EAGLE_RADIOBUTTON, MSG_DRAW, 0);
}


static void disable_stretching (void)
{
    NO_STRETCH_RADIOBUTTON.flags &=~ D_DISABLED;
    PLAIN_STRETCH_RADIOBUTTON.flags |= D_DISABLED;
    SUPER_2XSAI_RADIOBUTTON.flags |= D_DISABLED;
    SUPER_EAGLE_RADIOBUTTON.flags |= D_DISABLED;

    broadcast_dialog_message (MSG_RADIO, STRETCH_METHOD_RADIO_GROUP);
    NO_STRETCH_RADIOBUTTON.flags |= D_SELECTED;

    redraw_stretching_mode_radiobuttons ();
}


static void enable_stretching (void)
{
    NO_STRETCH_RADIOBUTTON.flags |= D_DISABLED;
    PLAIN_STRETCH_RADIOBUTTON.flags &=~ D_DISABLED;
    SUPER_2XSAI_RADIOBUTTON.flags &=~ D_DISABLED;
    SUPER_EAGLE_RADIOBUTTON.flags &=~ D_DISABLED;

    if (NO_STRETCH_RADIOBUTTON.flags & D_SELECTED) {
	object_message (&NO_STRETCH_RADIOBUTTON, MSG_RADIO, STRETCH_METHOD_RADIO_GROUP);
	PLAIN_STRETCH_RADIOBUTTON.flags |= D_SELECTED;
    }

    redraw_stretching_mode_radiobuttons ();
}


static int test_sound (void)
{
    SAMPLE *sample = store_get_dat ("/basic/explosion/explo42/sound");

    play_sample (sample, SFX_SLIDER.d2, 128, 1000, FALSE);

    return D_O_K;
}


static int music_slider_changed (DIALOG *d)
{
    music_desired_volume = (float)d->d2 / 255.0;

    return D_O_K;
}


static void update_gamma_preview (float gamma)
{
    BITMAP *src = store_get_dat ("/frontend/menu/gamma-preview");

    blit (src, GAMMA_PREVIEW.dp, 0, 0, 0, 0, src->w, src->h);
    apply_gamma (GAMMA_PREVIEW.dp, gamma);
}


static int gamma_slider_changed (DIALOG *d)
{
    update_gamma_preview (gamma_slider_d2_to_gamma_factor (d->d2));
    object_message (&GAMMA_PREVIEW, MSG_DRAW, 0);

    return D_O_K;
}


static int options_menu_modify_changes_pressed (void)
{
    int w, h, method;

    /* Screen resolution & stretching method. */
    if (RESOLUTION_640x400_RADIOBUTTON.flags & D_SELECTED)
	w = 640, h = 400;
    else if (RESOLUTION_640x480_RADIOBUTTON.flags & D_SELECTED)
	w = 640, h = 480;
    else
	w = 320, h = 200;
    method = radiobuttons_to_stretch_method ();

    /* Sound & music. */
    sound_volume_factor = SFX_SLIDER.d2 / 255.0;
    music_allowed = MUSIC_CHECKBOX.flags & D_SELECTED;

    /* Brightness. */
    if (GAMMA_SLIDER.d2 != old_desired_brightness) {
	alert ("You must restart Red Pixel for brightness to take effect.", 
	       NULL, NULL, "Ok", NULL, 0, 0);
	gamma_factor = gamma_slider_d2_to_gamma_factor (GAMMA_SLIDER.d2);
    }

    /* Test the desired video mode if it has been changed. */
    if ((desired_game_screen_w != old_desired_game_screen_w) ||
	(desired_game_screen_h != old_desired_game_screen_h)) {
	if (setup_video (w, h, 0) == 0) {
	    desired_game_screen_w = w;
	    desired_game_screen_h = h;
	    stretch_method_in_use = method;

	    if (video_mode_is_big (w, h)) {
		desired_menu_screen_w = w;
		desired_menu_screen_h = h;

		lobby_shutdown ();
		lobby_init (NULL);
		fancy_gui_shutdown ();
		fancy_gui_init ();
	    }
	    else
		set_menu_gfx_mode ();

	    screen_blitter_shutdown ();
	    screen_blitter_init (method, bitmap_color_depth (screen));
	}
	else {
	    set_menu_gfx_mode ();
	    show_mouse (screen);
	    alert ("Couldn't set the desired video mode.", NULL, NULL, "Ok", NULL, 0, 0);
	    return D_REDRAW;
	}
    }

    return D_EXIT;
}


static int cancel_changes_pressed (void)
{
    broadcast_dialog_message (MSG_RADIO, SCREEN_RESOLUTION_RADIO_GROUP);
    broadcast_dialog_message (MSG_RADIO, STRETCH_METHOD_RADIO_GROUP);
    music_desired_volume = old_music_desired_volume;

    return D_EXIT;
}


static DIALOG options_menu[] =
{
    { fancy_bitmap_proc,     0,   0, 640, 480, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 0 */
    { fancy_button_proc,    20,  20, 295, 170, 0, -1, 0, D_DISABLED, 0, 0, NULL, NULL, NULL }, /* 1 */
    { fancy_label_proc,     30,  20, 150,  30, 0, -1, 0, 0, 0, 0xa0, "Screen resolution:", NULL, NULL }, /* 2 */
    { fancy_radio_proc,     30,  60, 100,  30, 0, -1, 0, 0, SCREEN_RESOLUTION_RADIO_GROUP, 0x80, "320x200", NULL, disable_stretching }, /* 3 */
    { fancy_radio_proc,     30,  90, 100,  30, 0, -1, 0, 0, SCREEN_RESOLUTION_RADIO_GROUP, 0x80, "640x400", NULL, enable_stretching }, /* 4 */
    { fancy_radio_proc,     30, 120, 100,  30, 0, -1, 0, 0, SCREEN_RESOLUTION_RADIO_GROUP, 0x80, "640x480", NULL, enable_stretching }, /* 5 */
    { fancy_button_proc,   325,  20, 295, 170, 0, -1, 0, D_DISABLED, 0, 0, NULL, NULL, NULL }, /* 6 */
    { fancy_label_proc,    340,  20, 160,  30, 0, -1, 0, 0, 0, 0xa0, "Stretching method:", NULL, NULL }, /* 7 */
    { fancy_radio_proc,    340,  60, 160,  30, 0, -1, 0, 0, STRETCH_METHOD_RADIO_GROUP, 0x60, "No stretching", NULL, NULL }, /* 8 */
    { fancy_radio_proc,    340,  90, 160,  30, 0, -1, 0, 0, STRETCH_METHOD_RADIO_GROUP, 0x60, "Plain stretching", NULL, NULL },	/* 9 */
    { fancy_radio_proc,    340, 120, 160,  30, 0, -1, 0, 0, STRETCH_METHOD_RADIO_GROUP, 0x60, "Super 2xSaI", NULL, NULL }, /* 10 */
    { fancy_radio_proc,    340, 150, 160,  30, 0, -1, 0, 0, STRETCH_METHOD_RADIO_GROUP, 0x60, "Super Eagle", NULL, NULL }, /* 11 */
    { fancy_label_proc,    150, 220, 100,  30, 0, -1, 0, 0, 0, 0xa0, "SFX", NULL, NULL }, /* 12 */
    { fancy_slider_proc,   220, 220, 300,  30, 0,  0, 0, 0, 255, 0, NULL, NULL, NULL }, /* 13 */
    { fancy_button_proc,   540, 220,  60,  30, 0, -1, 0, 0, 0, 0x80, "Play", NULL, test_sound }, /* 14 */
    { fancy_checkbox_proc, 120, 260, 100,  30, 0, -1, 0, 0, 0, 0xa0, "Music", NULL, NULL }, /* 15 */
    { fancy_slider_proc,   220, 260, 300,  30, 0,  0, 0, 0, 255, 0, NULL, music_slider_changed, NULL }, /* 16 */
    { fancy_label_proc,    120, 300, 100,  30, 0, -1, 0, 0, 0, 0xa0, "Brightness", NULL, NULL }, /* 17 */
    { fancy_slider_proc,   220, 300, 300,  30, 0,  0, 0, 0, 10, 0, NULL, gamma_slider_changed, NULL }, /* 18 */
    { fancy_bitmap_proc,   540, 300,  32,  32, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL }, /* 19 */
    { fancy_button_proc,   360, 340, 100,  40, 0, -1, 0, 0, 0, 0x80, "Ok", NULL, options_menu_modify_changes_pressed },	/* 20 */
    { fancy_button_proc,   480, 340, 110,  40, 0, -1, 0, 0, 0, 0x80, "Cancel", NULL, cancel_changes_pressed }, /* 21 */
    { d_yield_proc,          0,   0,   0,   0, 0,  0, 0, 0, 0, 0, NULL, NULL, NULL },
    { NULL }
};


void options_menu_run (void)
{
    /* Screen resolution & stretching method. */
    if (video_mode_is_big (desired_game_screen_w, desired_game_screen_h)) {
	if (desired_game_screen_h == 400)
	    RESOLUTION_640x400_RADIOBUTTON.flags |= D_SELECTED;
	else
	    RESOLUTION_640x480_RADIOBUTTON.flags |= D_SELECTED;
	enable_stretching ();
    }
    else {
	RESOLUTION_320x200_RADIOBUTTON.flags |= D_SELECTED;
	disable_stretching ();
    }
    stretch_method_to_radiobuttons (stretch_method_in_use);
    old_desired_game_screen_w = desired_game_screen_w;
    old_desired_game_screen_h = desired_game_screen_h;

    /* Sound & music. */
    SFX_SLIDER.d2 = sound_volume_factor * 255;
    if (music_allowed)
	MUSIC_CHECKBOX.flags |= D_SELECTED;
    else
	MUSIC_CHECKBOX.flags &= ~D_SELECTED;
    MUSIC_SLIDER.d2 = music_desired_volume * 255;
    old_music_desired_volume = music_desired_volume;

    /* Brightness. */
    GAMMA_SLIDER.d2 = gamma_factor_to_gamma_slider_d2 (gamma_factor);
    old_desired_brightness = GAMMA_SLIDER.d2;
    update_gamma_preview (gamma_factor);

    fancy_do_dialog (options_menu, DEFAULT_FOCUS);
}


int options_menu_init (BITMAP *background)
{
    int fg = makecol (0xff, 0xff, 0xff);
    int bg = makecol (0xbf, 0x8f, 0x3f);

    initialize_fancy_dialog (options_menu, background, fg, bg);

    GAMMA_PREVIEW.dp = create_bitmap (GAMMA_PREVIEW.w, GAMMA_PREVIEW.h);

    return 0;
}


void options_menu_shutdown (void)
{
    destroy_bitmap (GAMMA_PREVIEW.dp);

    shutdown_fancy_dialog (options_menu);
}


/*------------------------------------------------------------*/
/* Config files.                                              */
/*------------------------------------------------------------*/

#define CONFIG_FILENAME		"redpixel.cfg"
#define CONFIG_SECTION		"redpixel2"


void load_config (int *desired_stretch_method)
{
    int stretch_method;

    push_config_state ();
    set_config_file (CONFIG_FILENAME);

    /* Screen resolution & stretching method. */
    desired_game_screen_w = get_config_int (CONFIG_SECTION, "screen_width", 320);
    desired_game_screen_h = get_config_int (CONFIG_SECTION, "screen_height", 200);
    stretch_method = get_config_int (CONFIG_SECTION, "stretch_method", 0);

    /* Sound & music. */
    sound_volume_factor = get_config_float (CONFIG_SECTION, "sound_volume_factor", 1.0);
    music_allowed = get_config_int (CONFIG_SECTION, "music_allowed", 1);
    music_desired_volume = get_config_float (CONFIG_SECTION, "music_volume_factor", 1.0);

    /* Brightness. */
    gamma_factor = get_config_float (CONFIG_SECTION, "gamma", 1.0);
    gamma_factor = MID (0.5, gamma_factor, 1.5);

    /* Other stuff. */
    strcpy (name_editbox_buf, get_config_string (CONFIG_SECTION, "name", "Gutless"));
    strcpy (address_editbox_buf, get_config_string (CONFIG_SECTION, "server", "localhost"));
    strcpy (port_editbox_buf, get_config_string (CONFIG_SECTION, "port", DEFAULT_PORT));


    /* Test the desired video mode.  Change if necessary. */
    if (autodetect_video_mode (&desired_game_screen_w, &desired_game_screen_h, 1) < 0)
	error ("Couldn't detect a suitable video mode.  Supported modes are:\n 320x200, 640x400, 640x480\n");

    if (video_mode_is_big (desired_game_screen_w, desired_game_screen_h)) {
	desired_menu_screen_w = desired_game_screen_w;
	desired_menu_screen_h = desired_game_screen_h;
    }

    /* Make sure the stretching method is okay for the screen mode. */
    if (video_mode_is_big (desired_game_screen_w, desired_game_screen_h)) {
	if ((stretch_method < STRETCH_METHOD_PLAIN) || 
	    (stretch_method > STRETCH_METHOD_SUPEREAGLE))
	    stretch_method = STRETCH_METHOD_PLAIN;
    }
    else
	stretch_method = STRETCH_METHOD_NONE;
    stretch_method_in_use = stretch_method;
    *desired_stretch_method = stretch_method;


    pop_config_state ();
}


void save_config (void)
{
    push_config_state ();
    set_config_file (CONFIG_FILENAME);

    /* Screen resolution & stretching method. */
    set_config_int (CONFIG_SECTION, "screen_width", desired_game_screen_w);
    set_config_int (CONFIG_SECTION, "screen_height", desired_game_screen_h);
    set_config_int (CONFIG_SECTION, "stretch_method", stretch_method_in_use);

    /* Sound & music. */
    set_config_float (CONFIG_SECTION, "sound_volume_factor", sound_volume_factor);
    set_config_int (CONFIG_SECTION, "music_allowed", music_allowed);
    set_config_float (CONFIG_SECTION, "music_volume_factor", music_desired_volume);

    /* Brightness. */
    set_config_float (CONFIG_SECTION, "gamma", gamma_factor);

    /* Other stuff. */
    set_config_string (CONFIG_SECTION, "name", name_editbox_buf);
    set_config_string (CONFIG_SECTION, "server", address_editbox_buf);
    set_config_string (CONFIG_SECTION, "port", port_editbox_buf);

    flush_config_file ();
    pop_config_state ();
}
