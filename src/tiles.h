#ifndef _included_tiles_h_
#define _included_tiles_h_

#include "convtbl.h"

typedef unsigned short tile_t;

#define TILE_BLANK	0xffff


extern struct convtable *tiles;


void create_tiles_table(int size);     /* $-export */
void add_tiles_pack(char *filename);   /* $-export */
void destroy_tiles_table(void);

#endif
