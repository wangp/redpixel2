#ifndef __included_cursor_h
#define __included_cursor_h


int cursor_init ();
void cursor_shutdown ();
void cursor_set_magic_bitmap (BITMAP *bmp, int hotx, int hoty);
void cursor_set_dot ();
void cursor_set_default ();


#endif
