/* path.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include "path.h"


char *path_share[] = {
    "data/",
    "/usr/local/share/redpixel2/",
    "/usr/share/redpixel2/",
    0
};


void path_init (void)
{
    /*
      XXX:
       - add in home directories, etc. 
       - unicodify
       - check search order
    */
}


void path_shutdown (void)
{
}
