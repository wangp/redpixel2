/* sound.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "fastsqrt.h"
#include "sound.h"


/* XXX
  what's supposed to happen is that we allocate voices
  and update the panning/volume with respect to the position
  between the client object and obj

  also, we need to be able to let lua code control
  start/stop/loop of the voices

  voices are released when they are stopped, or the owner
  object is destroyed
*/


static int ref_x, ref_y;


void sound_update_reference_point (int x, int y)
{
    ref_x = x;
    ref_y = y;
}


void sound_play_once (SAMPLE *spl, int origin_x, int origin_y)
{
    float dx, dy;
    float dist;
    int vol, pan;

    dx = origin_x - ref_x;
    dy = origin_y - ref_y;

    dist = fast_fsqrt (dx * dx + dy * dy);
    if (dist == 0)
	vol = 255;
    else
	vol = 255 * MIN (1.0, (1 / dist) * 64);

    pan = dx / 8 + 128;
    pan = MID (0, pan, 255);
    
    play_sample (spl, vol, pan, 1000, FALSE);
}
