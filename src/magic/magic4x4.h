#ifndef __included_magic4x4_h
#define __included_magic4x4_h


#include <allegro.h>


/* magic4x4.c */

BITMAP *get_magic_bitmap_format(BITMAP *orig, PALETTE pal);
BITMAP *unget_magic_bitmap_format(BITMAP *orig);

void generate_magic_color_map(void);
void free_magic_color_map(void);
void generate_magic_conversion_tables(void);
void blit_magic_format(BITMAP *bmp, BITMAP *dest, int w, int h);
void set_magic_bitmap_brightness(BITMAP *bmp, int r, int g, int b);
void set_magic_bitmap_brightness_skipping_black(BITMAP *bmp, int r, int g, int b);

BITMAP *create_magic_bitmap(int w, int h);
void draw_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y);
void draw_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y);
void draw_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int color);


/* magichf.c */

void draw_magic_sprite_h_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);


/* magicrot.c */

void pivot_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_scaled_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void pivot_scaled_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void rotate_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_scaled_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);
void rotate_scaled_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);

void pivot_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_scaled_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void pivot_scaled_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void rotate_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_scaled_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);
void rotate_scaled_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);

void pivot_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, int color);
void pivot_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, int color);
void pivot_scaled_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale, int color);
void pivot_scaled_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale, int color);
void rotate_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, int color);
void rotate_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, int color);
void rotate_scaled_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale, int color);
void rotate_scaled_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale, int color);


#endif
