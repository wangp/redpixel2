/* gameloop.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "fps.h"
#include "gameloop.h"
#include "magic4x4.h"
#include "render.h"
#include "store.h"		/* XXX */


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


#define TICK_RATE	60


static void run_object_processes ()
{
    object_t *p;
    lua_Object table;
    lua_Object process;

    lua_beginblock ();
		    
    for (p = map->objects.next; p; p = p->next) {
	table = lua_getref (p->type->table);
	if (!lua_istable (table))
	    continue;
	
	lua_pushobject (table);
	lua_pushstring ("process");
	process = lua_gettable ();
	if (!lua_isfunction (process))
	    continue;

	/* process (self) : (no output) */
	lua_pushobject (lua_getref (p->self));
	lua_callfunction (process);
    }

    lua_endblock ();
}


static void do_move (object_t *p);

static void move_objects ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next)
	do_move (p);
}


/*
**	XXX - player movement test
*/

static object_t *any_object_collide (object_t *p, bitmask_t *mask, int x, int y)
{
    object_t *q;

    for (q = map->objects.next; q; q = q->next)
	if ((p != q) && bitmask_check_collision (mask, q->mask, x, y, q->x, q->y))
	    return q;

    return 0;
}


#define MASS		1.0
#define GRAVITY		0.83 
#define AIRDRAG		0.6

static void do_move (object_t *p)
{
#define GET_MASK(x, y)	((x->y) ? (x->y) : (x->mask))
    
    bitmask_t *top    = GET_MASK (p, mask_top);
    bitmask_t *bottom = GET_MASK (p, mask_bottom);
    bitmask_t *left   = GET_MASK (p, mask_left);
    bitmask_t *right  = GET_MASK (p, mask_right);

#undef GET_MASK
    
    float dx, dy;
    int i;

    /* XXX: these belong elsewhere */
    {
	/* not on solid ground: fall */
	if (!bitmask_check_collision (map->tile_mask, bottom, 0, 0, p->x, p->y + 1))
	    p->yv += GRAVITY * MASS; /* XXX: p->mass */

	/* terminal velocity due to wind resistance */
	if (p->yv > 0) {
	    p->yv -= AIRDRAG;
	    if (p->yv < 0)
		p->yv = 0;
	}
    }
    
#define DIV	10

    dx = p->xv / DIV;
    dy = p->yv / DIV;

    for (i = 0; (i < DIV) && (dx || dy); i++) {
	if (dx < 0) {
	    if (!bitmask_check_collision (map->tile_mask, left, 0, 0, p->x + dx, p->y))
		p->x += dx;
	    else
		p->xv = dx = 0;
	}
	else if (dx > 0) {
	    if (!bitmask_check_collision (map->tile_mask, right, 0, 0, p->x + dx, p->y))
		p->x += dx;
	    else
		p->xv = dx = 0;
	}

    	if (dy < 0) {
	    if (!bitmask_check_collision (map->tile_mask, top, 0, 0, p->x, p->y + dy))
		p->y += dy;
	    else
		p->yv = dy = 0;
	}
	else if (dy > 0) {
	    if (!bitmask_check_collision (map->tile_mask, bottom, 0, 0, p->x, p->y + dy))
		p->y += dy;
	    else
		p->yv = dy = 0;
	}
    }
    
#undef DIV
}

static void move_player (object_t *p)
{
    p->xv *= 0;			/* XXX: implement tile friction */
}

#define JUMP_PERIOD	60/7

static int jump;

static void try_jump (object_t *p)
{
    bitmask_t *bottom = store_dat ("/player/mask-bottom");

    if (bitmask_check_collision (map->tile_mask, bottom, 0, 0, p->x, p->y + 1)) {
	p->yv = -4;
	jump = 0;
    }
    else {
	if (++jump < JUMP_PERIOD)
	    p->yv -= 1.2;
    }
}

static void try_move_horiz (object_t *p, bitmask_t *mask, float xv)
{
    int i, j, ramp;
    int sign, nogood;

    if (bitmask_check_collision (map->tile_mask, store_dat ("/player/mask-bottom"), 0, 0, p->x, p->y +  1))
	ramp = 6;
    else
	ramp = 3;

    sign = (xv < 0) ? -1 : +1;
    
    for (i = 0; i <= ramp; i++) {
	nogood = 0;

	for (j = 0; j < ABS ((int) xv); j++)
	    if (bitmask_check_collision (map->tile_mask, mask, 0, 0, p->x + (sign * j), p->y - i)) {
		nogood = 1;
		break;
	    }

	if (!nogood) {
	    p->y -= i;
	    p->xv += xv;
	    if ((i == 0) && (ramp == 6))
		goto down;
	    break;
	}
    }

    return;

  down:

    for (i = 1; i <= 7; i++) 
	if (bitmask_check_collision (map->tile_mask, mask, 0, 0, p->x, p->y + i)) {
	    i--;
	    break;
	}

    if (i < 8)
	p->y += i;
}
    
static void try_left (object_t *p)
{
    try_move_horiz (p, store_dat ("/player/mask-left"), -2);
}

static void try_right (object_t *p)
{
    try_move_horiz (p, store_dat ("/player/mask-right"), 2);
}

/*
**	end player movement test
*/


static void animate_objects ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next)
	if (p->render == OBJECT_RENDER_MODE_ANIM) 
	    object_anim_advance (p->anim);
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


void game_loop ()
{
    BITMAP *dbuf;
/* XXX vars */
    camera_t camera;
    int white;
    object_t *player;
    light_t *light;
/* end XXX */
    int quit = 0, draw = 1;
    int t;

    dbuf = create_magic_bitmap (SCREEN_W, SCREEN_H);

    /* XXX */ {
	white = makecol (0xff, 0xff, 0xff);

	player = object_create ("player");
	player->x = map->starts.next->x;
	player->y = map->starts.next->y;
	map_link_object (map, player);

	light = map_light_create (map, player->x - 32, player->y - 32, store_index ("/lights/white-small"));
    } /* end XXX */

    tick_init (TICK_RATE);
    fps_init ();

    while (!quit) {
	t = tick;
	while (t > 0) {
	    /* XXX */ {
		if (key[KEY_UP]) try_jump (player);
		else jump = 100;
		if (key[KEY_LEFT])  try_left (player);
		if (key[KEY_RIGHT]) try_right (player);
		

		move_objects ();
		move_player (player);

		camera.y = player->y - 100;
		camera.x = player->x - 160;
		light->x = player->x - 10;
		light->y = player->y - 10;

		{
		    object_t *q;

		    q = any_object_collide (player, player->mask, player->x, player->y);
		    if (q) {
			object_t *p = q;
			
			lua_beginblock ();

			if (lua_istable (lua_getref (p->type->table))) {
			    lua_Object proc;

			    lua_pushobject (lua_getref (p->type->table));
			    lua_pushstring ("player_touched");
			    proc = lua_gettable ();

			    if (lua_isfunction (proc)) {
				lua_pushobject (lua_getref (p->self));
				lua_pushobject (lua_getref (player->self));
				lua_callfunction (proc);
			    }
			}
			
			lua_endblock ();

			//map_object_destroy (map, q);
		    }
		}
	    } /* end XXX */

	    run_object_processes ();
	    animate_objects ();
	    purge_object_list ();
		
	    t--, tick--;
	    draw = 1;
	}

	if (draw) {
	    render (dbuf, map, &camera);
	    blit_magic_format (dbuf, screen, SCREEN_W, SCREEN_H);
	    textprintf (screen, font, 0, 0, white, "%d FPS", fps); /* XXX */

	    frames++;
	    draw = 0;
	}
	
	if (key[KEY_Q])
	    quit = 1;
    }

    fps_shutdown ();
    tick_shutdown ();

    destroy_bitmap (dbuf);
}
