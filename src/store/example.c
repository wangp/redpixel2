/* example for Store (does not do anything) */


#include <allegro.h>
#include "store.h"


int main ()
{
    int idx;
    BITMAP *bmp;
    
    allegro_init ();
    if (set_gfx_mode (GFX_AUTODETECT, 320, 200, 0, 0) < 0)
	return 1;
    
    /* Initialise Store, passing it the size of the hash table you
     * want to use.  */
    store_init (200);

    /* Load in some datafiles, inserting them in different parts of
     * the virtual directory structure.  Note that items from
     * `tiles2.dat' will override items from `tiles1.dat', unless
     * `tiles2.dat' is unloaded.  Nested datafiles are handled
     * properly: items will be separated by a `/' character.
     *
     * Error checking has been omitted.
     */
    store_load ("tiles1.dat", "/tiles/"); 
    store_load ("tiles2.dat", "/tiles/");
    store_load ("chars.dat", "/characters/");

    /* Get an index into the `store' data structure, which you can use
     * as shown.  */
    idx = store_index ("/tiles/bricks/001");
    if (idx)
	draw_sprite (screen, store[idx]->dat, 0, 0);
 
    /* ... another way to do it.  */
    bmp = store_datafile ("/tiles/bricks/002")->dat;
    if (bmp)
	draw_sprite (screen, bmp, 0, 0);
 
    /* .. and another.  */
    bmp = store_dat ("/tiles/bricks/003");
    if (bmp)
	draw_sprite (screen, bmp, 0, 0);
 
    /* Shutdown and unload datafiles.  */
    store_shutdown ();

    return 0;
}

END_OF_MAIN ();


/*
 * end of example.c
 */
