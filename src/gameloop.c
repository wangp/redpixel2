/* gameloop.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "console.h"
#include "fps.h"
#include "gameloop.h"
#include "gamenet.h"
#include "net.h"
#include "magic4x4.h"
#include "map.h"
#include "object.h"
#include "render.h"
#include "yield.h"
#include "vars.h"


#define TICK_RATE	60


object_t *local_player;

int game_quit;


/*----------------------------------------------------------------------*/

static volatile unsigned long tick;

static void tick_ticker ()
{
    tick++;
}

END_OF_STATIC_FUNCTION (tick_ticker);


static void tick_init (int bps)
{
    LOCK_VARIABLE (tick);
    LOCK_FUNCTION (tick_ticker);
    install_int_ex (tick_ticker, BPS_TO_TIMER (bps));
}


static void tick_shutdown ()
{
    remove_int (tick_ticker);
}


/*----------------------------------------------------------------------*/


#define MASS		1.0
#define GRAVITY		0.83
#define AIRDRAG		0.6


#define mask_top(obj)		(obj->mask_top    ? obj->mask_top    : obj->mask)
#define mask_bottom(obj)	(obj->mask_bottom ? obj->mask_bottom : obj->mask)
#define mask_left(obj)  	(obj->mask_left   ? obj->mask_left   : obj->mask)
#define mask_right(obj) 	(obj->mask_right  ? obj->mask_right  : obj->mask)


static int check_collision (bitmask_t *mask, int x, int y)
{
    return bitmask_check_collision (map->tile_mask, mask, 0, 0, x, y);
}


static int stand_on_solid (object_t *p)
{
    return check_collision (mask_bottom (p), p->x, p->y + 1);
}


static void apply_gravity (object_t *p)
{
    if (!stand_on_solid (p))
	p->yv += GRAVITY * MASS;
}


static void apply_air_drag (object_t *p)
{
    if (p->yv > 0)
	p->yv = MAX (0, p->yv - AIRDRAG);
}


static void do_move_object_smart (object_t *p)
{
    const int div = 10;
    float dx, dy;
    int i;

    /* Apply pseudo-gravity.  */
    apply_gravity (p);

    /* Apply air-drag.  */
    apply_air_drag (p);

    /* Move according to velocity.  */
    dx = p->xv / div;
    dy = p->yv / div;

    for (i = 0; (i < div) && (dx || dy); i++) {
	if (dx) {
	    if (!check_collision ((dx < 0) ? mask_left (p) : mask_right (p), p->x + dx, p->y))
		p->x += dx;
	    else {
		p->xv = dx = 0;
		p->dirty = 1;	/* unexpected */
	    }
	}

	if (dy) {
	    if (!check_collision ((dy < 0) ? mask_top (p) : mask_bottom (p), p->x, p->y + dy))
		p->y += dy;
	    else {
/************************************************************************/

		{
		    lua_Object self;
	    
		    lua_beginblock ();

		    self = lua_getref (p->self);
		    lua_pushobject (self);
		    lua_pushstring ("jump");
		    if (lua_isnumber (lua_rawgettable ())) {
			lua_pushobject (self);
			lua_pushstring ("jump");
			lua_pushnumber (1000);
			lua_rawsettable ();
		    }	    

		    lua_endblock ();
		}
 
/************************************************************************/		

		p->yv = dy = 0;
		p->dirty = 1;
	    }
	}
    }
}


static void move_objects_smart ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next)
	do_move_object_smart (p);
}


/* Replicate object variables if they have deviated from their
   simulated course (i.e. `dirty' flag has been set).  */
static void replicate_objects ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next) 
	if (p->dirty) {
	    gamenet_server_replicate_variable_change (p->id, p->x, p->y, p->xv, p->yv);
	    p->dirty = 0;
	}
}


static void purge_object_list ()
{
    object_t *p, *next;

    for (p = map->objects.next; p; p = next) {
	next = p->next;
	if (p->dying) {
	    map_unlink_object (map, p);
	    object_destroy (p);
	}
    }
}


static void do_logic ()
{
/****************************************************/
    if (server) {
	lua_Object self;
	int num;
	object_t *p = local_player;

	if (key[KEY_RIGHT])
	    p->xv = +2, p->dirty = 1;
	else if (key[KEY_LEFT]) 
	    p->xv = -2, p->dirty = 1;
	else if (stand_on_solid (p) && p->xv)
	    p->xv = 0, p->dirty = 1;
	else if (p->xv > 0)
	    p->xv = MAX (0, p->xv - 0.25), p->dirty = 1;
	else if (p->xv < 0)
	    p->xv = MIN (0, p->xv + 0.25), p->dirty = 1;
	
	if (key[KEY_UP]) {
	    lua_beginblock ();

	    self = lua_getref (p->self);
	    lua_pushobject (self);
	    lua_pushstring ("jump");
	    num = lua_getnumber (lua_rawgettable ());

	    if (num < 11 && (num || stand_on_solid (p))) {
		if (!check_collision (mask_top (p), p->x, p->y - 1)) {
		    p->yv -= 1.3;
		    p->dirty = 1;

		    lua_pushobject (self);
		    lua_pushstring ("jump");
		    lua_pushnumber (num + 1);
		    lua_rawsettable ();
		}
	    }	    

	    lua_endblock ();
	}
	else if (stand_on_solid (p)) {
	    lua_beginblock ();
	    lua_pushobject (lua_getref (p->self));
	    lua_pushstring ("jump");
	    lua_pushnumber (0);
	    lua_rawsettable ();
	    lua_endblock ();
	}
    }
/****************************************************/

    move_objects_smart ();
    purge_object_list ();
}


static void do_draw (BITMAP *dbuf)
{
    camera_t camera = { local_player->x + 8 - 160, local_player->y + 8 - 100};
    light_t *light = map_light_create (map, local_player->x, local_player->y,
				       store_index ("/lights/white-small"));
					       
    if (client) {
	camera.x = 8 * 16;
	camera.y = 4 * 16;
    }

    render (dbuf, map, &camera);

    map_light_destroy (map, light);
}


int game_loop_update (BITMAP *dbuf)
{
    int draw = 0;
    int t;

    if (game_quit)
	return -1;

    t = tick;
    while (t > 0) {
	if (client)
	    if (net_client_poll () < 0)
		game_quit = 1;
	
	do_logic ();

	t--, tick--;
	draw = 1;
    }
    
    if (server) {
	replicate_objects ();
	
	if (net_server_poll () < 0)
	    game_quit = 1;
    }
    
    if (draw) {
	do_draw (dbuf);
	frames++;
    }

    return 0;
}


int game_loop_init ()
{
    tick_init (TICK_RATE);
    fps_init ();
    
    game_quit = 0;

    return 0;
}


void game_loop_shutdown ()
{
    fps_shutdown ();
    tick_shutdown ();
}
