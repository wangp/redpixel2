#ifndef __included_rottrans_h
#define __included_rottrans_h


void pivot_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);
void pivot_scaled_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void pivot_scaled_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, int cx, int cy, fixed angle, fixed scale);
void rotate_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle);
void rotate_scaled_trans_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);
void rotate_scaled_trans_magic_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y, fixed angle, fixed scale);


#endif
