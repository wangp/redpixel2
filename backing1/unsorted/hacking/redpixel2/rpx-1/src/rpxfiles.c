/* rpx.c : stuff to do with .RPX files
 */

#include "rpxfiles.h"
#include "tiles.h"
#include "report.h"


rpx_t rpx;


/* put a string, then \0 */
static int my_pack_fputs(char *p, PACKFILE *f)
{
    int l = pack_fputs(p, f);
    pack_putc('\0', f);
    return l+1;
}

 
/* modified from file.c - read until MAX, or \n or EOF or \0 reached */
static char *my_pack_fgets(char *p, int max, PACKFILE *f)
{
   int c;

   if (pack_feof(f)) {
      p[0] = 0;
      return NULL;
   }

   for (c=0; c<max-1; c++) {
      p[c] = pack_getc(f);
      if (p[c] == '\r')
	 c--;
      else if (p[c] == '\n' || p[c] == '\0')
	 break;
   }

   p[c] = 0;

   if (errno)
      return NULL;
   else
      return p;
}



/* reset_rpx:
 *  Reset RPX to default values.
 */
void reset_rpx()
{
    int x, y;
    
    strcpy(rpx.name, "title-of-rpx");
    
    rpx.w = 64;
    rpx.h = 64;
    
    for (y=0; y<RPX_MAX_HEIGHT; y++) {
	for (x=0; x<RPX_MAX_WIDTH; x++) {
	    rpx.tile[y][x] = TILE_BLANK;
	}
    }
}



/* load_rpx:
 *  Loads an RPX file, returning zero on success.
 */

static struct convtable *tbl = NULL;   /* Uh oh, non reentrant! */

static void _load_name(PACKFILE *fp)
{
    my_pack_fgets(rpx.name, 256, fp);
}

static void _load_size(PACKFILE *fp)
{
    rpx.w = pack_igetw(fp);
    rpx.h = pack_igetw(fp);
}

static void _load_tiletbl(PACKFILE *fp)
{
    char buf[256];
    
    tbl = construct_convtable();
    
    do {
	my_pack_fgets(buf, 256, fp);	
	add_convtable_item(tbl, buf, NULL);
    } while (strcmp(buf, "#end-chunk"));
}

static void _load_tiledat(PACKFILE *fp)
{
    int x, y, src, dest;
    
    if (!tbl) {
	errprintf("(!tbl while loading rpx!");
	exit(666);	       /* you should be shot. */
    }
	    
    for (y=0; y<rpx.h; y++)
    {
	for (x=0; x<rpx.w; x++)
	{
	    src = pack_igetw(fp);
	    if (src != TILE_BLANK) {
		dest = translate_convtable(tiles, tbl, src);
		rpx.tile[y][x] = (dest != -1) ? dest : TILE_BLANK;
	    } else
	      rpx.tile[y][x] = TILE_BLANK;
	}
    }
}
		   


/* the main load routine 
 * Returns 0 on success.
 */
int load_rpx(char *filename)
{
    PACKFILE *fp;
    unsigned long magic, chunk;
    
    fp = pack_fopen(filename, "r");
    if (!fp)
      return -1;
    
    magic = pack_igetl(fp);
    if (magic != RPX_MAGIC) {
	pack_fclose(fp);
	return -1;
    }
    
    reset_rpx();
    
    while (!pack_feof(fp)) 
    {
	chunk = pack_igetl(fp);
	
	switch (chunk)
	{
	  case RPX_NAME:	_load_name(fp); break;
	  case RPX_SIZE:	_load_size(fp); break;
	  case RPX_TILETBL:	_load_tiletbl(fp); break;	    
	  case RPX_TILEDAT:	_load_tiledat(fp); break;
	    
	  default:
	    errprintf("Unrecognised chunk type %08x!\n", chunk);
	    exit(1);
	}
    }
    
    if (tbl) {
	destroy_convtable(tbl, NULL);
	tbl = NULL;
    }
        
    return 0;
}



/* save_rpx:
 *  Write an RPX file to disk, returning zero on success.
 */

static void _save_name(PACKFILE *fp)		/* RPX_NAME */
{
    pack_iputl(RPX_NAME, fp);
    my_pack_fputs(rpx.name, fp);
}

static void _save_size(PACKFILE *fp)		/* RPX_SIZE */
{
    pack_iputl(RPX_SIZE, fp);
    pack_iputw(rpx.w, fp);
    pack_iputw(rpx.h, fp);
}

static void _save_tiletbl(PACKFILE *fp)		/* RPX_TILETBL */
{
    int i;
    convitem_t *tbl;

    pack_iputl(RPX_TILETBL, fp);

    for (i=0; i<tiles->size; i++)
    {
	tbl = &tiles->tbl[i];
	pack_fputs(tbl->key, fp);
	pack_putc(0, fp); 
    }

    pack_fputs("#end-chunk", fp);
    pack_putc(0, fp);
}

static void _save_tiledat(PACKFILE *fp)		/* RPX_TILEDAT */
{
    int x, y;
    
    pack_iputl(RPX_TILEDAT, fp);

    for (y=0; y<rpx.h; y++)
    {
	for (x=0; x<rpx.w; x++)
	{
	    pack_iputw(rpx.tile[y][x], fp);
	}
    }
}

/* the main save routine */
int save_rpx(char *filename)
{
    PACKFILE *fp;
    fp = pack_fopen(filename, "w");
    if (!fp)
      return -1;
    
    pack_iputl(RPX_MAGIC, fp);

    _save_name(fp);
    _save_size(fp);    
    _save_tiletbl(fp);
    _save_tiledat(fp);

    pack_fclose(fp);
    return 0;
}



/*
 * rpx-object related : mostly for map editor 
 */


/* rpx_link_object:
 *  Add an object to the object l'list.
 */
void rpx_link_object(object_t *obj)
{    
    /* for speed we just stick it up the front */
    if (rpx.obj_head) {
	rpx.obj_head->prev = obj;
	obj->next = rpx.obj_head;
	obj->prev = NULL;
	rpx.obj_head = obj;
    } 
    else {
	rpx.obj_head = obj;
	obj->next = obj->prev = NULL;
    }    
}


/* rpx_unlink_object:
 *  Delete OBJ from the list.
 *  Does NOT free the memory.
 */
void rpx_unlink_object(object_t *obj)
{
    if (obj == rpx.obj_head) {
	rpx.obj_head = obj->next;
	if (rpx.obj_head)
	  rpx.obj_head->prev = NULL;
	return;
    }
    
    if (obj->prev)
      obj->prev->next = obj->next;
    
    if (obj->next)
      obj->next->prev = obj->prev;
}



