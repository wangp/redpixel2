/* scripts/init.sc	-*- mode: C -*- */

#author "Psyk"
#title "init.sc"

#include "scripts/allegro.sh"
#include "scripts/imports.sh"

export init;


/* init:
 *  Top level initialisation routine, used in both map editor and 
 *  game engine.  Returns zero on success.
 */
int init()
{   
    /* tiles */
    create_tiles_table(200);
    add_tiles_pack("data/tiles.dat");
            
    /* weapons */
    add_script("scripts/rpg.sc");
    //add_script("scripts/shotgun.sc");

    return 0;
}
