#ifndef _included_tiles_h_
#define _included_tiles_h_

#include <ppcol.h>
#include "convtbl.h"

typedef unsigned short tile_t;

#define TILE_BLANK	0xffff

typedef struct tiledata		       /* to be pointed to by convtbl->tbl[x]->data */
{
    BITMAP *bmp;
    PPCOL_MASK *mask;
} tiledata_t;

extern struct convtable *tiles;


void create_tiles_table();
void destroy_tiles_table(void);
void add_tiles_pack(char *filename);  


BITMAP *tile_bmp(int i);

#endif
