/* mapfile.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "map.h"
#include "mapfile.h"
#include "objtypes.h"
#include "store.h"


static int pack_fputs_nl (const char *s, PACKFILE *f)
{
    int size;

    size = pack_fputs (s, f);
    if (size != EOF)
	if (pack_putc ('\n', f) != EOF)
	    return size + 1;

    return EOF;
}


#define ID(a,b,c,d)	(a | (b << 8) | (c << 16) | (d << 24))

#define HEADER_MAGIC	ID ('p', 'i', 't', 'r')
#define HEADER_VERSION	0x0100
#define HEADER_SILLY	"God... root... what is difference?"

#define MARK_TILES	ID ('t', 'i', 'l', 'e')
#define MARK_LIGHTS	ID ('l', 'i', 't', 'e')
#define MARK_OBJECTS	ID ('o', 'b', 'j', 'c')
#define MARK_STARTS 	ID ('s', 't', 'r', 't')


/*----------------------------------------------------------------------*/


int map_save (map_t *map, const char *filename)
{
    PACKFILE *f;

    f = pack_fopen (filename, F_WRITE_PACKED);
    if (!f) goto error;

    /* Header.  */
    if ((pack_iputl (HEADER_MAGIC, f) == EOF)
	|| (pack_iputl (HEADER_VERSION, f) == EOF)
	|| (pack_fputs_nl (HEADER_SILLY, f) == EOF))
	goto error;

    /* Map dimensions.  */
    if ((pack_iputw (map->width, f) == EOF)
	|| (pack_iputw (map->height, f) == EOF))
	goto error;
    
    /* Tiles.  */
    if (pack_iputl (MARK_TILES, f) == EOF)
	goto error;

    {
	int x, y, t;
	
	for (y = 0; y < map->height; y++)
	    for (x = 0; x < map->width; x++) {
		t = map->tile[y][x];

		if (t) {
		    if (pack_fputs_nl (store_key (t), f) == EOF)
			goto error;
		}
		else {
		    if (pack_fputs_nl ("()", f) == EOF)
			goto error;
		}
	    }
    }

    /* Lights.  */
    {
	light_t *p;
	int num;
	
	num = 0;
    	foreach (p, map->lights) num++;

	if (num > 0) {
	    if ((pack_iputl (MARK_LIGHTS, f) == EOF)
		|| (pack_iputl (num, f) == EOF))
		goto error;
	
	    foreach (p, map->lights)
		if ((pack_iputl (p->x, f) == EOF)
		    || (pack_iputl (p->y, f) == EOF)
		    || (pack_fputs_nl (store_key (p->lightmap), f) == EOF))
		    goto error;
	}
    }

    /* Objects.  */
    {
	object_t *p;
	int num;

	num = 0;
	foreach (p, map->objects) num++;

	if (num > 0) {
	    if ((pack_iputl (MARK_OBJECTS, f) == EOF)
		|| (pack_iputl (num, f) == EOF))
		goto error;
	
	    foreach (p, map->objects) {
		const char *name = p->type->name;
		
		if ((pack_fputs_nl (name, f) == EOF)
		    || (pack_iputl (p->cvar.x, f) == EOF)
		    || (pack_iputl (p->cvar.y, f) == EOF))
		    goto error;
	    }
	}
    }

    /* Starts.  */
    {
	start_t *p;
	int num;

	num = 0;
	foreach (p, map->starts) num++;

	if (num > 0) {
	    if ((pack_iputl (MARK_STARTS, f) == EOF)
		|| (pack_iputl (num, f) == EOF))
		goto error;
	
	    foreach (p, map->starts)
		if ((pack_iputl (p->x, f) == EOF)
		    || (pack_iputl (p->y, f) == EOF))
		    goto error;
	}
    }

    pack_fclose (f);
    return 0;
    
  error:
    
    if (f) pack_fclose (f);
    return -1;
}


/*----------------------------------------------------------------------*/

  
static int read_tiles (map_t *map, PACKFILE *f)
{
    int x, y, t;
    char tmp[1024];
    int warning = 0;

    for (y = 0; y < map->height; y++)
	for (x = 0; x < map->width; x++) {
	    if (!pack_fgets (tmp, sizeof tmp, f))
		goto error;
	    
	    if (ustrcmp (tmp, "()") == 0)
		t = 0;
	    else {
		t = store_index (tmp);
		if (!t) warning = 1;
	    }
	    
	    map->tile[y][x] = t;
	}

    return warning;

  error:

    return -1;
}


static int read_lights (map_t *map, PACKFILE *f)
{
    int num, i;
    char tmp[1024];
    int x, y, idx;
    int warning = 0;

    num = pack_igetl (f);
    if (num == EOF) goto error;

    for (i = 0; i < num; i++) {
	x = pack_igetl (f);
	y = pack_igetl (f);
	if ((x == EOF) || (y == EOF))
	    goto error;

	if (!pack_fgets (tmp, sizeof tmp, f))
	    goto error;
	idx = store_index (tmp);
	if (idx == 0)
	    warning = 1;
	else
	    map_light_create (map, x, y, idx);
    }

    return warning;

  error:

    return -1;
}


static int read_objects (map_t *map, PACKFILE *f, int loadobjects)
{
    int num, i;
    char tmp[1024];
    int x, y;
    object_t *p;

    num = pack_igetl (f);
    if (num == EOF) goto error;

    for (i = 0; i < num; i++) {
	if (!pack_fgets (tmp, sizeof tmp, f))
	    goto error;
	    
	x = pack_igetl (f);
	y = pack_igetl (f);
	if ((x == EOF) || (y == EOF))
	    goto error;

	/* If we are the client, skip object creation as the server
	 * will send replicate them to us anyway.  */
	if (loadobjects) {
	    p = object_create (tmp);
	    if (!p)
		goto error;
	    else {
		p->cvar.x = x;
		p->cvar.y = y;
		map_link_object (map, p);
	    }
	}
    }

    return 0;

  error:

    return -1;
}


static int read_starts (map_t *map, PACKFILE *f)
{
    int num, i;
    int x, y;
    int warning = 0;

    num = pack_igetl (f);
    if (num == EOF) goto error;

    for (i = 0; i < num; i++) {
	x = pack_igetl (f);
	y = pack_igetl (f);
	if ((x == EOF) || (y == EOF))
	    goto error;

	map_start_create (map, x, y);
    }

    return warning;

  error:

    return -1;
}


map_t *map_load (const char *filename, int loadobjects, int *warning)
{
    PACKFILE *f;
    map_t *map = 0;
    int warn;

    if (!warning)
	warning = &warn;
    *warning = 0;

    f = pack_fopen (filename, F_READ_PACKED);
    if (!f) goto error;

    /* Header.  */
    {
	long magic, version;
	char tmp[1024];
	
	magic = pack_igetl (f);
	version = pack_igetl (f);
	pack_fgets (tmp, sizeof tmp, f);

	if ((magic != HEADER_MAGIC)
	    || (version != HEADER_VERSION))
	    goto error;
    }
    
    map = map_create ();
    if (!map) goto error;
    
    /* Map dimensions.  */
    {
	int width, height;

	width = pack_igetw (f);
	height = pack_igetw (f);

	if ((width == EOF) || (height == EOF))
	    goto error;

	map_resize (map, width, height);
    }

    /* Read chunks.  */
    while (!pack_feof (f)) {
	unsigned long chunk = pack_igetl (f);
	if (chunk == EOF)
	    goto error;

	switch (chunk) {
	    case MARK_TILES:
		warn = read_tiles (map, f);
		if (warn < 0) goto error;
		if (warn > 0) *warning = 1;
		break;

	    case MARK_LIGHTS:
		warn = read_lights (map, f);
		if (warn < 0) goto error;
		if (warn > 0) *warning = 1;
		break;

	    case MARK_OBJECTS:
		warn = read_objects (map, f, loadobjects);
		if (warn < 0) goto error;
		if (warn > 0) *warning = 1;
		break;

	    case MARK_STARTS:
		warn = read_starts (map, f);
		if (warn < 0) goto error;
		if (warn > 0) *warning = 1;
		break;

	    default:
		goto error;
	}
    }

    pack_fclose (f);

    map_generate_tile_mask (map);
    return map;

  error:

    if (f) pack_fclose (f);
    if (map) map_destroy (map);
    
    return 0;
}
