/* example.c
 *
 * Example for Good Wumpus Use.
 */

#include <stdio.h>
#include <allegro.h>
#include "wumpus.h"

int main ()
{
    WUMPUS *w;
    DATAFILE *d;
    BITMAP *bmp;
    
    allegro_init ();
    
    /* Call this if you want to use the `gellopy' feature of the
     * library (see README).  The number is an estimate of how big you
     * think the hash table needs to be (it does not matter if this
     * number is too small, but data accesses will work more slowly).
     */
    install_gellopy (200);
    
    /* Load the datafiles.  You only really to use `load_wumpus' if
     * you won't be using the `fat_gellopy' to retrieve data.
     */
    w = load_wumpus ("skeleton.dat");
    d = load_datafile ("grunt.dat");
    
    if ((!w) || (!d)) {
	allegro_message ("Error loading datafiles.\n");
	return 1;
    }
    
    /* Link the wumpus and datafile into the central hash table.
     * The base names could be anything, but this is the recommended
     * format (kind of like Unix directory structure).  The trailing
     * slash is optional.
     */
    link_wumpus (w, "/undead/skeleton");
    link_datafile (d, "/grunt/");
    
    /* Here we access data from a wumpus.  These lines are equivalent.  */
    bmp = wumpus_datafile (w, "character/standing/frame0")->dat;
    bmp = wumpus_dat (w, "character/standing/frame0");
    
    /* Now we access data from the gellopy (i.e. central hash table).  */
    bmp = gellopy_dat ("/grunt/character/attack1/frame10");
    bmp = gellopy_dat ("/undead/skeleton/character/standing/frame0");

    /* The exit handler `remove_gellopy' will automatically unlink the
     * datafiles but you still need to unload the data manually.
     */
    unload_wumpus (w);
    unload_datafile (d);    
    
    return 0;
}

END_OF_MAIN();

/*
 * example.c ends here
 */
