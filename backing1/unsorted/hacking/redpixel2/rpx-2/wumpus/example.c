/* example for Good Wumpus Use */

#include <stdio.h>

/* include these */
#include <allegro.h>
#include "wumpus.h"

int main()
{
    WUMPUS *w;
    DATAFILE *d;
    BITMAP *bmp;
    
    allegro_init();
    
    /* call this if you want to use the gellopy feature of wumpus */
    install_gellopy(200); 	       /* we want 200 entries! */
    
    /* load the datafiles */
    w = load_wumpus("skeleton.dat");
    d = load_datafile("grunt.dat");
    
    if (!w || !d) {
	printf("You need to create some datafiles yourself.\n");
	return 1;
    }
    
    /* linking wumpii and datafiles into fat_gellopy:
     * note that the trailing slash is optional.
     * 
     * the base names could be anything, but this is the recommended format
     * (kinda like unix directory structure)
     */
    link_wumpus(w, "/undead/skeleton");
    link_datafile(d, "/grunt/");
    
    /* accessing data from a wumpus 
     * these are equivalent 
     */
    bmp = wumpus_datafile(w, "character/standing/frame0")->dat;
    bmp = wumpus_dat(w, "character/standing/frame0");
    
    /* accessing data from the gellopy 
     */
    bmp = gellopy_dat("/grunt/character/attack1/frame10");
    bmp = gellopy_dat("/undead/skeleton/character/standing/frame0");
        
    /* remove_gellopy will automagically unlink the datafiles
     * but you need to unload the data manually
     */
    unload_wumpus(w);
    unload_datafile(d);    
    
    /* remove_gellopy kicks in here */
    return 0;
}

END_OF_MAIN();
