/* scripts/imports.sh	-*- mode: C -*- 
 *	Be sure to keep this in sync with src/ *.h
 */


/* inclued automagically generated file: _imports.sh */

#include "scripts/_imports.sh"


/* structures */

struct player_t {
    char *name;
};

struct object_t {
    char *name;
};


/* typedefs */

typedef void (*weapon_handler_t)(msg_t msg, player_t *pl);
typedef void (*object_handler_t)(msg_t msg, object_t *obj, player_t *pl);

