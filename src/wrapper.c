/* wrapper.c : wrapper functions to be exported to SeeR scripts.
 * 	We don't allow SeeR scripts to know the insides of structures
 * 	therefore we provide functions for that purpose.
 */

#include <allegro.h>



/*
 * 
 *	DATAFILEs
 *
 */



/* df_get_item_num:
 *  Returns DATAFILE index corresponding to NAME.
 */
int df_get_item_num(DATAFILE *dat, char *name)
{
    DATAFILE *entry;
    int i = 0;
    
    if (!dat)
      return -1;
    
    entry = &dat[0];
    while (entry->type != DAT_END)
    {
	if (strcmp(name, get_datafile_property(entry, DAT_NAME)) == 0)
	  return i;
	else
	  entry = &dat[++i];
    }
    
    return -1;    
}


/* df_get_item:
 *  Returns DATAFILE * corresponding to NAME.
 */
DATAFILE *df_get_item(DATAFILE *dat, char *name)
{
    int i = df_get_item_num(dat, name);
    if (i == -1)
      return NULL;
    else
      return &dat[i];
}


/* df_item:
 *  Returns DATAFILE * corresponding to index I.
 */
DATAFILE *df_item(DATAFILE *dat, unsigned int i)
{
    if  (!dat) return NULL;
    return &dat[i];
}


/* df_dat:
 *  Returns D -> dat.
 */
void *df_dat(DATAFILE *d)
{
    if (!d) return NULL;
    return d->dat;
}


/* df_count:
 *  Returns number of entries of TYPE (-1 = *) in datafile.
 */
int df_count(DATAFILE *dat, int type)
{
    DATAFILE *entry;
    int i = 0, c = 0;
    
    if (!dat)
      return 0;
    
    entry = &dat[0];
    while (entry->type != DAT_END)
    {
	if (type == -1 || dat->type == type)
	  c++;
	entry = &dat[++i];
    }
    
    return c;  
}


/* df_foreach:
 *  Runs a callback for each entry in a datafile.
 *  Returns non-zero on sucess, zero on error.
 */
int df_foreach(DATAFILE *dat, int (*callback)(DATAFILE *d))
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



/*
 * 
 *	BITMAP
 * 
 */



/* bmp_line:
 *  Returns BMP -> line[row], or NULL if invalid.
 */
unsigned char *bmp_line(BITMAP *bmp, unsigned int row)
{
    if (!bmp || row >= bmp->h) 
      return NULL;
    else
      return bmp->line[row];
}


/* bmp_w:
 *  Returns bitmap width.
 */
int bmp_w(BITMAP *b)
{
    if (!b) return 0;
    return b->w;
}


/* bmp_h:
 *  Returns bitmap height.
 */
int bmp_h(BITMAP *b)
{
    if (!b) return 0;
    return b->h;
}

