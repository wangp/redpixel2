#ifndef __included_mapfile_h
#define __included_mapfile_h


#include "map.h"


int map_save (map_t *map, const char *filename);
map_t *map_load (const char *filename);


#endif
