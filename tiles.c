/* tiles.c : stuff to do with tiles, and their conv tables, datafiles, etc. 
 */

#include <allegro.h>

#include "report.h"
#include "df.h"
#include "hash.h"
#include "convtbl.h"


struct convtable *tiles;


/* create_tiles_hash_table: (SeeR exported)
 * destroy_tiles: (not exported)
 *  Do what they are called.
 */
void create_tiles_table(int size)
{
    tiles = construct_convtable(size);
}

static void _destroy_cb(void *d)
{
    if (d)
      destroy_bitmap((BITMAP*)d);
}

void destroy_tiles_table(void)
{
    destroy_convtable(tiles, _destroy_cb);
}


/* add_tiles_pack: (SeeR exported)
 *  Reads a datafile, loading in each BMP as a new tile, generating entries
 *  in the conversion table. 
 */
static char *_fn;

static int _add_cb(DATAFILE *e)
{
    char *prop;
    BITMAP *b1, *b2;
    char buf[256];
    
    if (e->type == DAT_BITMAP) 
    {
	prop = get_datafile_property(e, DAT_NAME);
	sprintf(buf, "%s#%s", _fn, prop);

	b1 = e->dat;	
	b2 = create_bitmap(b1->w, b1->h);
	blit(b1, b2, 0, 0, 0, 0, b1->w, b2->h);	

	add_convtable_item(tiles, buf, b2);
		    
	debugprintf("\t%s\n", buf);
    }    
    
    return 0;
}

void add_tiles_pack(char *filename)
{
    DATAFILE *dat = load_datafile(filename);
    if (!dat)
      return;
    
    debugprintf("adding %s...\n", filename);
    
    _fn = filename;
    df_foreach(dat, _add_cb);
    
    unload_datafile(dat);
}
