#ifndef __included_textout_h
#define __included_textout_h


struct BITMAP;
struct FONT;


void textout_magic (
    struct BITMAP *, struct FONT *, const char *, int x, int y, int color);
void textout_right_magic (
    struct BITMAP *, struct FONT *, const char *, int x, int y, int color);
void textout_centre_trans_magic (
    struct BITMAP *, struct FONT *, const char *, int x, int y, int color);
void textprintf_trans_magic (
    struct BITMAP *, struct FONT *, int x, int y, int col, const char *, ...);
void textprintf_right_trans_magic (
    struct BITMAP *, struct FONT *, int x, int y, int col, const char *, ...);


#endif
