/* loaddata.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "loadhelp.h"
#include "vtree.h"


static void *tiles;

void tiles_load ()
{
    tiles = loadhelp_load ("tile/*.dat", VTREE_TILES);
}

void tiles_unload ()
{
    loadhelp_unload (tiles);
}

void tiles_enumerate (void (*proc) (const char *filename, int id))
{
    loadhelp_enumerate (tiles, proc);
}


/*------------------------------------------------------------*/


static void *lights;

void lights_load ()
{
    lights = loadhelp_load ("light/*.dat", VTREE_LIGHTS);
}

void lights_unload ()
{
    loadhelp_unload (lights);
}

void lights_enumerate (void (*proc) (const char *filename, int id))
{
    loadhelp_enumerate (lights, proc);
}
