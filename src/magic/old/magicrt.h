#ifndef __included_magicrt_h
#define __included_magicrt_h


void pivot_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_trans_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_scaled_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void pivot_scaled_trans_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void rotate_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_trans_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_scaled_trans_sprite_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale, int h_flip, int v_flip);
void rotate_scaled_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);
void rotate_scaled_trans_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);


#endif
