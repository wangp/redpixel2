#ifndef _included_tiles_h_
#define _included_tiles_h_

#include "convtbl.h"

typedef unsigned short tile_t;


extern struct convtable *tiles;


void create_tiles_table(int size);
void destroy_tiles_table(void);
void add_tiles_pack(char *filename);

#endif
