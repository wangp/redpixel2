#ifndef __included_magic4x4_h
#define __included_magic4x4_h


BITMAP *get_magic_bitmap_format(BITMAP *orig, PALETTE pal);
void generate_magic_conversion_tables();
void blit_magic_format_to_screen(BITMAP *bmp);
void set_magic_bitmap_brightness(BITMAP *bmp, int r, int g, int b);

BITMAP *create_magic_bitmap(int w, int h);
void draw_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y);


#endif
