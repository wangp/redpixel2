#ifndef __included_set_video_h
#define __included_set_video_h

int setup_video (int w, int h, int d);
int autodetect_video_mode (int *w, int *h, int allow_low_res);

#endif
