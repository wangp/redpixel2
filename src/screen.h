#ifndef __included_screen_h
#define __included_screen_h


/*
 * These aliases are for readability only.  Don't expect to be able to
 * change these and have everything magically working.  We don't use
 * SCREEN_W, SCREEN_H because we might be rendering to a 320x200
 * bitmap before stretching up to a high-resolution video mode.
 *
 * Reiterating: SCREEN_W, SCREEN_H means the for-real hardware video
 * mode dimensions.  screen_width, screen_height are the dimensions of
 * any (most) double buffer bitmaps that we render to.
 */

#define screen_width	320
#define screen_height	200


#define STRETCH_METHOD_NONE	   0
#define STRETCH_METHOD_PLAIN	   1
#define STRETCH_METHOD_SUPER2XSAI  2
#define STRETCH_METHOD_SUPEREAGLE  3

extern int desired_game_screen_w, desired_game_screen_h;


void screen_blitter_init (int stretch_method, int colour_depth);
void screen_blitter_shutdown (void);
void blit_magic_bitmap_to_screen (struct BITMAP *);


#endif
