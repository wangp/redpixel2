#ifndef __included_mapfile_h
#define __included_mapfile_h


struct map;


int map_save (struct map *map, const char *filename);
struct map *map_load (const char *filename, int is_client, int *warning);


#endif
