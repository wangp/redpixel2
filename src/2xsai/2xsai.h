#ifndef __included_2xsai_h
#define __included_2xsai_h

#include <inttypes.h>

int Init_2xSaI(int depth);
void Super2xSaI(BITMAP * src, BITMAP * dest, int s_x, int s_y, int d_x, int d_y, int w, int h);
void Super2xSaI_ex(uint8_t *src, uint32_t src_pitch, uint8_t *unused, BITMAP *dest, uint32_t width, uint32_t height);

void SuperEagle(BITMAP * src, BITMAP * dest, int s_x, int s_y, int d_x, int d_y, int w, int h);
void SuperEagle_ex(uint8_t *src, uint32_t src_pitch, uint8_t *unused, BITMAP *dest, uint32_t width, uint32_t height);

#endif
