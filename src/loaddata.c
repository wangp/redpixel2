/* loaddata.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "loadhelp.h"
#include "vtree.h"


static void *tiles;

void tiles_load (void (*hook) (const char *filename, int id))
{
    tiles = loadhelp_load ("tile/*.dat", VTREE_TILES, hook);
}

void tiles_unload ()
{
    loadhelp_unload (tiles);
}


/*------------------------------------------------------------*/


static void *lights;

void lights_load (void (*hook) (const char *filename, int id))
{
    lights = loadhelp_load ("light/*.dat", VTREE_LIGHTS, hook);
}

void lights_unload ()
{
    loadhelp_unload (lights);
}

