#ifndef __included_sound_h
#define __included_sound_h


struct SAMPLE;

extern float sound_volume_factor;


void sound_update_reference_point (int x, int y);
void sound_play_once (struct SAMPLE *spl, int origin_x, int origin_y);


#endif
