#include <allegro.h>


int setup_video (int w, int h, int d)
{
    int depths[] = { 16, 15, 0 }, *i;
    
    if ((w == SCREEN_W) && (h == SCREEN_H))
	return 0;

    set_color_conversion (COLORCONV_NONE);
   
    if (d > 0) {
	set_color_depth (d);
	if (set_gfx_mode (GFX_AUTODETECT, w, h, 0, 0) == 0)
	    return 0;
    }
    else {
        for (i = depths; *i; i++) {
	    set_color_depth (*i);
	    if (set_gfx_mode (GFX_AUTODETECT, w, h, 0, 0) == 0)
		return 0;
	}
    }
    
    return -1;
}


int autodetect_video_mode (int *w, int *h, int allow_low_res)
{
    int d = screen ? bitmap_color_depth (screen) : 0;

    if (setup_video (*w, *h, d) == 0)
	return 0;
    else if ((allow_low_res) && (setup_video (320, 200, 0) == 0)) {
	*w = 320;
	*h = 200;
	return 0;
    }
    else if (setup_video (640, 400, 0) == 0) {
	*w = 640;
	*h = 400;
	return 0;
    }
    else if (setup_video (640, 480, 0) == 0) {
	*w = 640;
	*h = 480;
	return 0;
    }

    return -1;
}
