/* gameloop.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "fps.h"
#include "game.h"
#include "gameloop.h"
#include "gamenet.h"
#include "net.h"
#include "magic4x4.h"
#include "map.h"
#include "object.h"
#include "render.h"
#include "yield.h"


#define TICK_RATE	60


/* Perhaps this should be in game.c?  */
object_t *local_player;


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


static void apply_air_drag (object_t *p)
{
    if (p->yv > 0)
	p->yv = MAX (0, p->yv - AIRDRAG);
}


static void move_objects_simple ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next) {
	apply_air_drag (p);
	p->x += p->xv;
	p->y += p->yv;
    }
}


static void do_move_object_smart (object_t *p)
{
    const int div = 10;
    float dx, dy;
    int i;

    /* Apply pseudo-gravity.  */
    if (!stand_on_solid (p))
	p->yv += GRAVITY * MASS;

    /* Apply air-drag.  */
    apply_air_drag (p);

    /* Move according to velocity.  */
    dx = p->xv / div;
    dy = p->yv / div;

    for (i = 0; (i < div) && (dx || dy); i++) {
	if (dx) {
	    if (!check_collision ((dx < 0) ? mask_left (p) : mask_right (p), p->x + dx, p->y))
		p->x += dx;
	    else
		p->xv = dx = 0;
	}
	if (dy) {
	    if (!check_collision ((dy < 0) ? mask_top (p) : mask_bottom (p), p->x, p->y + dy))
		p->y += dy;
	    else
		p->yv = dy = 0;
	}
    }
}


static void move_objects_smart ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next) {
/****************************************************/

	if (key[KEY_RIGHT]) p->xv += 0.5;
	if (key[KEY_LEFT]) p->xv -= 0.5;

/****************************************************/
	
	do_move_object_smart (p);
    }
}


/* Save object variables which may need to be replicated so we can see
   if they have changed later.  */
static void save_objects ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next) {
	/* This is the simulated movement the client would be doing.  */
	p->old_x = p->x + p->xv;
	p->old_y = p->y + p->yv;
	p->old_xv = p->xv;
	p->old_yv = p->yv;
    }
}


/* Replicate object variables if they have deviated from their
   simulated course.  */
static void replicate_objects ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next) 
	if ((p->x  != p->old_x)  || (p->y  != p->old_y) ||
	    (p->xv != p->old_xv) || (p->yv != p->old_yv))
	    gamenet_server_replicate_variable_change (p->id, p->x, p->y, p->xv, p->yv);
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


static void logic ()
{
    if (server)
	move_objects_smart ();
    else
	move_objects_simple ();
    
    purge_object_list ();
}


static int preamble ()
{
    /* XXX: We need a way to break out of this.  */

    local_player = 0;

    if (server) {
	gamenet_server_send_game_state ();
    }

    if (client && (!server)) {
	gamenet_client_receive_game_state ();
    }

    return 0;
}


void game_loop ()
{
    BITMAP *dbuf;
    int quit = 0, draw = 1;
    int t;

    if (preamble () < 0)
	return;

    dbuf = create_magic_bitmap (SCREEN_W, SCREEN_H);
    tick_init (TICK_RATE);
    fps_init ();

    while (!quit && (!key[KEY_Q])) {
	if (server)
	    save_objects ();

	t = tick;
	while (t > 0) {
	    if (client)
		if (net_client_poll () < 0)
		    quit = 1;

	    logic ();

	    t--, tick--;
	    draw = 1;
	}

	if (server)
	    replicate_objects ();
	
	if (draw) {
	    camera_t camera = { local_player->x + 8 - 160, local_player->y + 8 - 100};
	    light_t *light = map_light_create (map, local_player->x, local_player->y, store_index ("/lights/white-small"));
					       
	    render (dbuf, map, &camera);
	    blit_magic_format (dbuf, screen, SCREEN_W, SCREEN_H);

	    map_light_destroy (map, light);

	    frames++;
	    draw = 0;
	}

	if (server)
	    if (net_server_poll () < 0)
		quit = 1;

	yield ();
    }

    fps_shutdown ();
    tick_shutdown ();
    destroy_bitmap (dbuf);
}
