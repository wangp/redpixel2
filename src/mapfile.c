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
    
	for (p = map->lights.next, num = 0; p; p = p->next)
	    num++;

	if (num > 0) {
	    if ((pack_iputl (MARK_LIGHTS, f) == EOF)
		|| (pack_iputl (num, f) == EOF))
		goto error;
	
	    for (p = map->lights.next; p; p = p->next)
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

	for (p = map->objects.next, num = 0; p; p = p->next)
	    num++;

	if (num > 0) {
	    if ((pack_iputl (MARK_OBJECTS, f) == EOF)
		|| (pack_iputl (num, f) == EOF))
		goto error;
	
	    for (p = map->objects.next; p; p = p->next) {
		const char *name = p->type->name;
		
		if ((pack_fputs_nl (name, f) == EOF)
		    || (pack_iputl (p->x, f) == EOF)
		    || (pack_iputl (p->y, f) == EOF))
		    goto error;
	    }
	}
    }

    /* Starts.  */
    {
	start_t *p;
	int num;

	for (p = map->starts.next, num = 0; p; p = p->next)
	    num++;

	if (num > 0) {
	    if ((pack_iputl (MARK_STARTS, f) == EOF)
		|| (pack_iputl (num, f) == EOF))
		goto error;
	
	    for (p = map->starts.next; p; p = p->next)
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


int read_tiles (map_t *map, PACKFILE *f)
{
    int x, y, t;
    char tmp[1024];

    for (y = 0; y < map->height; y++)
	for (x = 0; x < map->width; x++) {
	    if (!pack_fgets (tmp, sizeof tmp, f))
		goto error;
	    
	    if (ustrcmp (tmp, "()") == 0)
		t = 0;
	    else {
		t = store_index (tmp);
		if (!t) goto error;
	    }
	    
	    map->tile[y][x] = t;
	}

    return 0;

  error:

    return -1;
}


static int read_lights (map_t *map, PACKFILE *f)
{
    int num, i;
    char tmp[1024];
    int x, y, idx;

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
	if (idx == 0) goto error;
	    
	if (!map_light_create (map, x, y, idx))
	    goto error;
    }

    return 0;

  error:

    return -1;
}



static int read_objects (map_t *map, PACKFILE *f)
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

	p = object_create (tmp);
	if (!p)
	    goto error;
	else {
	    p->x = x;
	    p->y = y;
	    map_link_object (map, p);
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

    num = pack_igetl (f);
    if (num == EOF) goto error;

    for (i = 0; i < num; i++) {
	x = pack_igetl (f);
	y = pack_igetl (f);
	if ((x == EOF) || (y == EOF))
	    goto error;

	if (!map_start_create (map, x, y))
	    goto error;
    }

    return 0;

  error:

    return -1;
}


map_t *map_load (const char *filename)
{
    PACKFILE *f;
    map_t *map = 0;
    
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
		if (read_tiles (map, f) < 0)
		    goto error;
		break;

	    case MARK_LIGHTS:
		if (read_lights (map, f) < 0)
		    goto error;
		break;

	    case MARK_OBJECTS:
		if (read_objects (map, f) < 0)
		    goto error;
		break;

	    case MARK_STARTS:
		if (read_starts (map, f) < 0)
		    goto error;
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
