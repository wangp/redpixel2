#ifndef _included_object_h_
#define _included_object_h_

#include <allegro.h>
#include "defs.h"


typedef struct object_def
{
    int typenum;
    char *name;
    seer_addr handler;
    BITMAP *icon;
    
    struct object_def *prev, *next;
} object_def_t;


typedef struct object 
{
    object_def_t *def;
    unsigned int x, y;

    struct object *prev, *next;
} object_t;


#if 0				       /* SeeR exports only! */
/* $-start-export-block */
typedef void object_def_t;
typedef void object_t;
/* $-end-export-block */
#endif


int create_object_type(char *name, int handler, BITMAP *icon);	/* $-export */
object_def_t *get_object_type(int i);
void free_object_types();


struct object *create_object(struct object_def *def);
void destroy_object(struct object *obj);


#endif
