#ifndef _included_weapon_h_
#define _included_weapon_h_


#include "defs.h"
#include "player.h"
#include "object.h"


typedef void (*weapon_handler_t)(msg_t msg, player_t *pl);   /* $-export */
typedef void (*object_handler_t)(msg_t msg, object_t *obj, player_t *pl);/* $-export */


typedef struct weapon_type
{
    int typenum;
    char *name;
    seer_addr handler;
    
    struct weapon_type *prev, *next;
} weapon_type_t;


#if 0				       /* for autoexp only */
typedef void weapon_type_t;	       /* $-export */
#endif


int create_weapon_type(char *name);    /* $-export */
int assign_weapon_handler(int typenum, seer_addr handler);/* $-export */
void free_weapon_types();


#endif
