/* script.c : script helpers / wrappers
 */

#include <allegro.h>
#include "rpxfiles.h"


/* 
 * 	Keyboard stuff
 */

/* key_shifts:
 *  Helper for the key_shifts bitfield, as Lua doesn't seem to support & | ^
 *  etc.
 */
int __script_help__key_shifts(int flag)
{
    return key_shifts & flag;
}



/* 
 * 	DATAFILE stuff
 */

/* df:
 *  Returns DATAFILE index corresponding to NAME.
 */
void *__script_help__df(DATAFILE *d, int i)
{
    return d[i].dat;
}


/* df_item_num:
 *  Returns DATAFILE index corresponding to NAME.
 */
int __script_help__df_item_num(DATAFILE *dat, char *name)
{
    DATAFILE *entry;
    int i = 0;
    
    if (!dat)
      return -1;
    
    entry = &dat[0];
    while (entry->type != DAT_END) {
	if (strcmp(name, get_datafile_property(entry, DAT_NAME)) == 0)
	  return i;
	else
	  entry = &dat[++i];
    }
    
    return -1;    
}


/* df_item:
 *  Returns DATAFILE * corresponding to NAME.
 */
void *__script_help__df_item(DATAFILE *dat, char *name)
{
    int i = __script_help__df_item_num(dat, name);
    if (i == -1)
      return NULL;
    else
      return dat[i].dat;
}


/* df_count:
 *  Returns number of entries of TYPE (-1 = *) in datafile.
 */
int __script_help__df_count(DATAFILE *dat, int type)
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



/*
 *	BITMAP stuff
 */

/* bmp_line:
 *  Returns BMP -> line[row], or NULL if invalid.
 */
unsigned char *__script_help__bmp_line(BITMAP *bmp, unsigned int row)
{
    if (!bmp || row >= bmp->h) 
      return NULL;
    else
      return bmp->line[row];
}


/* bmp_w:
 *  Returns bitmap width.
 */
int __script_help__bmp_w(BITMAP *b)
{
    if (!b) return 0;
    return b->w;
}


/* bmp_h:
 *  Returns bitmap height.
 */
int __script_help__bmp_h(BITMAP *b)
{
    if (!b) return 0;
    return b->h;
}



/*
 * 	rpxfiles
 */

unsigned short __script_help__get_tile(int x, int y)
{
    return rpx.tile[y][x];
}

void __script_help__set_tile(int x, int y, int t)
{
    rpx.tile[y][x] = t;
}


/*
 *	Perform casting (toLua makes picky functions)
 */

BITMAP *__script_help__BITMAP(void *d)
{
    return (BITMAP *)d;
}

