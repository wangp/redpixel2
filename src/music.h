#ifndef __included_music_h
#define __included_music_h


extern int music_allowed;	   /* boolean */
extern float music_desired_volume; /* 0.0 to 1.0 */


void music_init (void);
void music_select_playlist (const char *filename);
void music_stop_playlist (void);
void music_shutdown (void);


#endif
