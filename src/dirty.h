#ifndef _included_dirty_h
#define _included_dirty_h

extern int not_dirty;

void mark_dirty(int x, int y, int w, int h);
void clear_dirty(BITMAP *dest, BITMAP *src);
void draw_dirty(BITMAP *dest, BITMAP *src);
void reset_dirty();

#endif
