#ifndef __included_magicrl_h
#define __included_magicrl_h


void pivot_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, int color);
void pivot_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, int color);
void pivot_scaled_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale, int color);
void pivot_scaled_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale, int color);
void rotate_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, int color);
void rotate_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, int color);
void rotate_scaled_lit_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale, int color);
void rotate_scaled_lit_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale, int color);


#endif
