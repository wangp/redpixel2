#ifndef __included_cursor_h
#define __included_cursor_h


struct BITMAP;


int cursor_init (void);
void cursor_shutdown (void);
void cursor_set_magic_bitmap (struct BITMAP *bmp, int hotx, int hoty);
void cursor_set_dot (void);
void cursor_set_default (void);


#endif
