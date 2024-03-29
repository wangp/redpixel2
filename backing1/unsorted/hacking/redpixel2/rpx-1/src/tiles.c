/* tiles.c : stuff to do with tiles, and their conv tables, datafiles, etc. 
 */

#include <allegro.h>
#include <ppcol.h>

#include "tiles.h"
#include "report.h"
#include "convtbl.h"


struct convtable *tiles;



/* df_foreach:
 *  Runs a callback for each entry in a datafile.
 *  Returns non-zero on sucess, zero on error.
 */
static int df_foreach(DATAFILE *dat, int (*callback)(DATAFILE *d))
{
    DATAFILE *entry;
    int i = 0;
    
    if (!dat)
      return 0;
    
    entry = &dat[0];
    while (entry->type != DAT_END)
    {
	if (callback(entry))
	  break;
	entry = &dat[++i];
    }
    
    return 1;
}


/* create_tiles_table:
 * destroy_tiles: 
 *  Do what they are called.
 */
void create_tiles_table()
{
    tiles = construct_convtable();
}

static void _destroy_cb(void *d)
{
    if (d) {
	destroy_bitmap(((tiledata_t *)d)->bmp);
	destroy_ppcol_mask(((tiledata_t *)d)->mask);
    }
}

void destroy_tiles_table(void)
{
    destroy_convtable(tiles, _destroy_cb);
}


/* add_tiles_pack:
 *  Reads a datafile, loading in each BMP as a new tile, generating entries
 *  in the conversion table. 
 */
static char *_fn;

static int _add_cb(DATAFILE *e)
{
    char *prop;
    struct tiledata *td;
    BITMAP *b1, *b2;
    char buf[256];
    
    if (e->type == DAT_BITMAP) 
    {
	prop = get_datafile_property(e, DAT_NAME);
	sprintf(buf, "%s#%s", _fn, prop);
	
	td = malloc(sizeof(struct tiledata));
	
	b1 = e->dat;	
	b2 = create_bitmap(b1->w, b1->h);
	blit(b1, b2, 0, 0, 0, 0, b1->w, b2->h);	

	td->bmp = b2;
	td->mask = create_ppcol_mask(b2);
	
	add_convtable_item(tiles, buf, td);
		    
	//debugprintf("\t%s\n", buf);
    }    
    
    return 0;
}

void add_tiles_pack(char *filename)
{
    DATAFILE *dat = load_datafile(filename);
    if (!dat)
      return;
    
    debugprintf("Adding %s...\n", filename);
    
    _fn = get_filename(filename);
    df_foreach(dat, _add_cb);
    
    unload_datafile(dat);
}


/* script helper */
BITMAP *tile_bmp(int i)
{
    struct tiledata *td;
    if (i < 0 || i >= tiles->size)
	return NULL;
    td = tiles->tbl[i].data;
    return td->bmp;
}
