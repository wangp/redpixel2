/* path.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "path.h"


char *path_share[] = {
    "data/",
    "/usr/local/share/redpixel2/",
    "/usr/share/games/redpixel2/",
    0
};


void path_init ()
{
    /*
      XXX:
       - add in home directories, etc. 
       - unicodify
       - check search order
    */
}


void path_shutdown ()
{
}