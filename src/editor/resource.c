/* resource.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "resource.h"
#include "alloc.h"


struct path {
    char *path;
    struct path *next;
};

static struct path search_path;


void resource_init ()
{
    char path[1024];   
    
    get_executable_name (path, sizeof path);
    add_resource_path (path);

    /*
      Data and scripts
      /usr/share/games/redpixel2/ 
      /usr/local/share/redpixel2/ 
      $HOME/.redpixel2/
      ./
      ../ (for src)            
    */
}

void resource_shutdown ()
{
    struct path *p, *next;
    
    for (p = search_path.next; p; p = next) {
	next = p->next;
	free (p->path);
	free (p);
    }
    
    search_path.next = 0;
}

void add_resource_path (char *path)
{
    struct path *p;
    char tmp[1024];
    
    p = alloc (sizeof *p);
    if (p) {
        fix_filename_path (tmp, path, sizeof tmp);
	p->path = ustrdup (tmp);
	p->next = search_path.next;
	search_path.next = p;
    }
}

char *find_resource (char *dest, char *filename, int size)
{
    struct path *p;

    for (p = search_path.next; p; p = p->next) {
	append_filename (dest, p->path, filename, sizeof dest);
	if (exists (dest))
	    return dest;
    }
    
    return 0;
}
