/* client.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <allegro.h>
#include "libnet.h"
#include "alloc.h"
#include "blod.h"
#include "camera.h"
#include "client.h"
#include "clsvface.h"
#include "error.h"
#include "fps.h"
#include "list.h"
#include "magic4x4.h"
#include "map.h"
#include "mapfile.h"
#include "mathcnst.h"
#include "messages.h"
#include "mylibnet.h"
#include "netmsg.h"
#include "object.h"
#include "packet.h"
#include "particle.h"
#include "render.h"
#include "screen.h"
#include "server.h"
#include "sound.h"
#include "store.h"
#include "sync.h"
#include "textout.h"
#include "timeout.h"


#if 0
# define dbg(msg)	puts ("[client] " msg)
#else
# define dbg(msg)
#endif


typedef unsigned char uchar_t;
typedef unsigned long ulong_t;

#define Lclt client_lua_namespace


/* our connection */
static NET_CONN *conn;
static int client_id;
static char *client_name;

/* for rendering */
static BITMAP *bmp;
static camera_t *cam;

static BITMAP *bkgd;
static int parallax_x = 2;
static int parallax_y = 2;

static BITMAP *crosshair;

/* the game state */
static map_t *map;
static object_t *local_object;
static object_t *tracked_object;
static int health;		/* for displaying only */
static int ammo;		/* the real values are on server */

/* network stuff */
static int pinging;
static ulong_t last_ping_time;
static int lag;
static int last_controls;
static float aim_angle;
static float last_aim_angle;

/* how bright the scoresheet is (0-15).  0 = disabled */
static int scores_brightness;



/*
 *----------------------------------------------------------------------
 *	Display switch callbacks
 *----------------------------------------------------------------------
 */


static int backgrounded;


static void switch_in_callback (void)
{
    backgrounded = 0;
}


static void switch_out_callback (void)
{
    backgrounded = 1;
}


static void display_switch_init (void)
{
    set_display_switch_callback (SWITCH_IN, switch_in_callback);
    set_display_switch_callback (SWITCH_OUT, switch_out_callback);
    backgrounded = 0;
}


static void display_switch_shutdown (void)
{
    remove_display_switch_callback (switch_out_callback);
    remove_display_switch_callback (switch_in_callback);
}



/*
 *----------------------------------------------------------------------
 *	Tick counter
 *----------------------------------------------------------------------
 */


/* keep in sync with server */
#define TICKS_PER_SECOND	(50)
#define MSECS_PER_TICK		(1000 / TICKS_PER_SECOND)


static volatile ulong_t ticks;


static void ticker (void)
{
    ticks++;
}

END_OF_STATIC_FUNCTION (ticker);


static void ticks_init (void)
{
    LOCK_VARIABLE (ticks);
    LOCK_FUNCTION (ticker);
    install_int_ex (ticker, BPS_TO_TIMER (TICKS_PER_SECOND));
    ticks = 0;
}


static void ticks_shutdown (void)
{
    remove_int (ticker);
}



/*
 *----------------------------------------------------------------------
 *	Remember some info about other clients
 *----------------------------------------------------------------------
 */


typedef struct client_info {
    struct client_info *next;
    struct client_info *prev;
    int id;
    char *name;
    char *score;
} client_info_t;


static list_head_t client_info_list;


static void client_info_list_init (void)
{
    list_init (client_info_list);
}


static void client_info_list_add (int id, const char *name, const char *score)
{
    client_info_t *c = alloc (sizeof *c);

    c->id = id;
    c->name = ustrdup (name);
    c->score = ustrdup (score);
    list_append (client_info_list, c);
}


static client_info_t *get_client_info (int id)
{
    client_info_t *c;

    list_for_each (c, &client_info_list)
	if (c->id == id)
	    return c;

    return NULL;
}


static void client_info_list_remove (int id)
{
    client_info_t *c = get_client_info (id);
    if (c)
	list_remove (c);
}


static void client_info_list_set_score (int id, const char *score)
{
    client_info_t *c = get_client_info (id);
    if (c) {
	free (c->score);
	c->score = ustrdup (score);
    }
}


static void _free_client_info (client_info_t *c)
{
    free (c->score);
    free (c->name);
    free (c);
}


static void client_info_list_free (void)
{
    list_free (client_info_list, _free_client_info);
}


/*
 * Process incoming client add/remove packets from the server.
 * These can come in at almost any time.
 */


static void process_sc_client_add (const char *buf)
{
    long id;
    short nlen;
    char name[NETWORK_MAX_PACKET_SIZE];
    short slen;
    char score[NETWORK_MAX_PACKET_SIZE];

    packet_decode (buf, "lss", &id, &nlen, name, &slen, score);
    client_info_list_add (id, name, score);
}


static void process_sc_client_remove (const char *buf)
{
    long id;
    packet_decode (buf, "l", &id);
    client_info_list_remove (id);
}



/*
 *----------------------------------------------------------------------
 *	Perform simple physics (simulation)
 *----------------------------------------------------------------------
 */


static void perform_simple_physics (ulong_t curr_ticks, int delta_ticks)
{
    list_head_t *object_list;
    object_t *obj;
    int i;

    object_list = map_object_list (map);
    list_for_each (obj, object_list) {
	if (!object_is_client_processed (obj)) {
	    object_do_simulation (obj, curr_ticks);
	    continue;
	}

	/*
	 * For CLIENT PROCESSED objects, we have to do some thinking
	 * about the physics ourselves, as if we were the server.  But
	 * we do not have accurate data to work with, so ONLY use
	 * proxy-only objects when:
	 *
	 *  (1) inaccurate results for this object don't matter, and
	 *  (2) this object will not influence other objects,
	 *	e.g. in collisions
	 *
	 * In short, this means eye-candy only.
	 */
	for (i = 0; i < delta_ticks; i++)
	    object_do_physics (obj, map);
    }

    for (i = 0; i < delta_ticks; i++) {
	particles_update (map_particles (map), map);
	map_explosions_update (map);
	map_blasts_update_visually_only (map);
    }
}



/*
 *----------------------------------------------------------------------
 *	Poll object update hooks
 *----------------------------------------------------------------------
 */


static void poll_update_hooks (int elapsed_msecs)
{
    list_head_t *object_list;
    object_t *obj;

    if (elapsed_msecs <= 0)
	return;

    object_list = map_object_list (map);
    list_for_each (obj, object_list)
	if (!object_stale (obj))
	    object_poll_update_hook (obj, elapsed_msecs);
}



/*
 *----------------------------------------------------------------------
 *	Send input controls to server
 *----------------------------------------------------------------------
 */


static void send_gameinfo_controls (void)
{
    int controls = 0;
    int update = 0;

    if (!messages_grabbed_keyboard ()) {
	if (key[KEY_A]) controls |= CONTROL_LEFT;
	if (key[KEY_D]) controls |= CONTROL_RIGHT;
	if (key[KEY_W]) controls |= CONTROL_UP;
	if (key[KEY_S]) controls |= CONTROL_DOWN;
	if (key[KEY_SPACE]) controls |= CONTROL_RESPAWN;
    }

    if (mouse_b & 1) controls |= CONTROL_FIRE;

    if (controls != last_controls)
	update = 1;

    if (last_aim_angle != aim_angle) {
	if (!update)
	    update = ((controls & CONTROL_FIRE)
		      ? (ABS (aim_angle - last_aim_angle) > (M_PI/256))
		      : (ABS (aim_angle - last_aim_angle) > (M_PI/16)));
	if (local_object)
	    object_set_number (local_object, "aim_angle", aim_angle);
    }

    if (update) {
	net_send_rdm_encode (conn, "cccf", MSG_CS_GAMEINFO,
			      MSG_CS_GAMEINFO_CONTROLS, controls, aim_angle);

	last_controls = controls;
	last_aim_angle = aim_angle;
    }
}


/* XXX stuff in the same packet as _CONTROLS? */
/* XXX a bit dirty this function */
static void send_gameinfo_weapon_switch (void)
{
    int keyboard_grabbed;
    int already_set = 0;

    keyboard_grabbed = messages_grabbed_keyboard ();

    /* KEY_1 - KEY_9 */
    {
	static char last_key[9] = {0,0,0,0,0,0,0,0,0};
	int i;

	for (i = 0; i < 9; i++) {
	    int is_down = key[KEY_1 + i];

	    if (!already_set && !keyboard_grabbed &&
		is_down && !last_key[i]) {
		lua_getglobal (Lclt, "weapon_order");
		lua_pushnumber (Lclt, i+1);
		lua_rawget (Lclt, -2);
		if (lua_isstring (Lclt, -1)) {
		    net_send_rdm_encode (conn, "ccs", MSG_CS_GAMEINFO,
					 MSG_CS_GAMEINFO_WEAPON_SWITCH,
					 lua_tostring (Lclt, -1));
		}
		lua_pop (Lclt, 1);
		already_set = 1;
	    }

	    last_key[i] = is_down;
	}
    }

    /* KEY_Q, KEY_E */
    {
	static int last_Q;
	static int last_E;

	int Q = key[KEY_Q];
	int E = key[KEY_E];

	int prev = !last_Q && Q;
	int next = !last_E && E;

	last_Q = Q;
	last_E = E;

	if (!already_set && !keyboard_grabbed) {
	    if (prev && !next) {
		net_send_rdm_encode (conn, "ccs", MSG_CS_GAMEINFO,
				     MSG_CS_GAMEINFO_WEAPON_SWITCH,
				     "_internal_prev");
		already_set = 1;
	    }
	    else if (next && !prev) {
		net_send_rdm_encode (conn, "ccs", MSG_CS_GAMEINFO,
				     MSG_CS_GAMEINFO_WEAPON_SWITCH,
				     "_internal_next");
		already_set = 1;
	    }
	}
    }

    /* mouse wheelie */
    {
	static int last_mouse_z;
	int dir = last_mouse_z - mouse_z;
	last_mouse_z = mouse_z;

	if (!already_set) {
	    if (dir > 0) {
		net_send_rdm_encode (conn, "ccs", MSG_CS_GAMEINFO,
				     MSG_CS_GAMEINFO_WEAPON_SWITCH,
				     "_internal_prev");
		already_set = 1;
	    }
	    else if (dir < 0) {
		net_send_rdm_encode (conn, "ccs", MSG_CS_GAMEINFO,
				     MSG_CS_GAMEINFO_WEAPON_SWITCH,
				     "_internal_next");
		already_set = 1;
	    }
	}
    }
}



/*
 *----------------------------------------------------------------------
 *	Process server-to-client gameinfo packets
 *----------------------------------------------------------------------
 */


#define SC_GAMEINFO_HANDLER(NAME)	\
static const uchar_t *NAME (const uchar_t *buf)


SC_GAMEINFO_HANDLER (sc_mapload)
{
    char filename[NETWORK_MAX_PACKET_SIZE];
    short len;

    buf += packet_decode (buf, "s", &len, filename);
    if (map)
	map_destroy (map);
    map = map_load (filename, 1, NULL);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_client_aim_angle)
{
    long id;
    float angle;
    object_t *obj;

    buf += packet_decode (buf, "lf", &id, &angle);
    if ((id != client_id) && (obj = map_find_object (map, id)))
	object_set_number (obj, "aim_angle", angle);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_client_status)
{
    long id;
    char type;
    long val;

    buf += packet_decode (buf, "lcl", &id, &type, &val);
    if (id == client_id) {	/* XXX a waste to broadcast this */
	switch (type) {
	    case 'h':
		health = val;
		break;
	    case 'a':
		ammo = val;
		break;
	    default:
		error ("unknown type in gameinfo client status packet (client)\n");
		break;
	}
    }

    return buf;
}


SC_GAMEINFO_HANDLER (sc_client_score)
{
    long id;
    char score[NETWORK_MAX_PACKET_SIZE];
    short len;

    buf += packet_decode (buf, "ls", &id, &len, score);
    client_info_list_set_score (id, score);
    return buf;
}


SC_GAMEINFO_HANDLER (sc_object_create)
{
    int top = lua_gettop (Lclt);

    char type[NETWORK_MAX_PACKET_SIZE];
    short len;
    objid_t id;
    char hidden;
    float x, y;
    float xv, yv;
    int ctag;
    object_t *obj;
    const char *realtype;

    /* decode the start of the packet */
    buf += packet_decode (buf, "slcffffc", &len, type, &id, &hidden,
			  &x, &y, &xv, &yv, &ctag);

    /* look up the object type alias */
    lua_getglobal (Lclt, "reverse_object_alias");
    lua_pushstring (Lclt, type);
    lua_rawget (Lclt, -2);
    if (lua_isstring (Lclt, -1))
	realtype = lua_tostring (Lclt, -1);
    else
	realtype = type;

    /* create proxy object */
    obj = object_create_proxy (realtype, id);
    if (!obj)
	error ("error: unable to create a proxy object (unknown type?)");
    if (hidden)
	object_hide (obj);
    else
	object_show (obj);
    object_set_auth_info (obj, ticks - lag, x, y, xv, yv, 0, 0);
    object_set_xy (obj, x, y);
    object_set_collision_tag (obj, ctag);
    if (id == client_id) {
	tracked_object = local_object = obj;
	object_set_number (obj, "is_local", 1);
    }

    /* decode optional extra fields */
    {
	char type;
	char name[NETWORK_MAX_PACKET_SIZE];
	short len;

	do {
	    buf += packet_decode (buf, "c", &type);
	    if (type == 'f') {
		float f;
		buf += packet_decode (buf, "sf", &len, name, &f);
		object_set_number (obj, name, f);
	    }
	    else if (type == 's') {
		char s[NETWORK_MAX_PACKET_SIZE];
		short slen;
		buf += packet_decode (buf, "ss", &len, name, &slen, &s);
		object_set_string (obj, name, s);
	    }
	    else if (type) {
		error ("error: unknown field type in object "
		       "creation packet (client)\n");
	    }
	} while (type);
    }

    /* if the object is of a client, set the name */
    if (object_is_client (obj)) {
	client_info_t *c = get_client_info (id);
	if (c)
	    object_set_string (obj, "name", c->name);
    }

    /* link and init */
    map_link_object (map, obj);
    object_run_init_func (obj);

    /* hack to get the camera to track a player's corpse */
    if (object_get_number (obj, "_internal_stalk_me") == client_id)
	tracked_object = obj;

    lua_settop (Lclt, top);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_object_destroy)
{
    objid_t id;
    object_t *obj;

    buf += packet_decode (buf, "l", &id);
    if ((obj = map_find_object (map, id))) {
	object_set_stale (obj);
	if (obj == local_object)
	    local_object = NULL;
	if (obj == tracked_object)
	    tracked_object = NULL;
    }

    return buf;
}


SC_GAMEINFO_HANDLER (sc_object_update)
{
    objid_t id;
    float x, y;
    float xv, yv;
    float xa, ya;
    object_t *obj;

    buf += packet_decode (buf, "lffffff", &id, &x, &y, &xv, &yv, &xa, &ya);
    if ((obj = map_find_object (map, id)))
	object_set_auth_info (obj, ticks - lag, x, y, xv, yv, xa, ya);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_object_hidden)
{
    objid_t id;
    char hidden;
    object_t *obj;

    buf += packet_decode (buf, "lc", &id, &hidden);
    if ((obj = map_find_object (map, id))) {
	if (hidden)
	    object_hide (obj);
	else
	    object_show (obj);
    }

    return buf;
}


SC_GAMEINFO_HANDLER (sc_object_call)
{
    long id;
    short method_len;
    char method[NETWORK_MAX_PACKET_SIZE];
    short arg_len;
    char arg[NETWORK_MAX_PACKET_SIZE];
    object_t *obj;

    buf += packet_decode (buf, "lss", &id, &method_len, method, &arg_len, arg);
    if ((obj = map_find_object (map, id))) {
	lua_pushstring (Lclt, arg);
	object_call (Lclt, obj, method, 1);
    }

    return buf;
}


SC_GAMEINFO_HANDLER (sc_particles_create)
{
    char type;
    float x;
    float y;
    long nparticles;
    float spread;
    void (*spawner)(particles_t *, float, float, long, float) = NULL;

    buf += packet_decode (buf, "cfflf", &type, &x, &y, &nparticles, &spread);
    switch (type) {
	case 'b': spawner = particles_spawn_blood; break;
	case 's': spawner = particles_spawn_spark; break;
	case 'r': spawner = particles_spawn_respawn_particles; break;
    }

    if (!spawner)
	error ("error: unknown particle type in gameinfo packet (client)\n");
    else
	(*spawner) (map_particles (map), x, y, nparticles, spread);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_blod_create)
{
    float x;
    float y;
    long nparticles;

    buf += packet_decode (buf, "ffl", &x, &y, &nparticles);
    blod_spawn (map, x, y, nparticles);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_explosion_create)
{
    char name[NETWORK_MAX_PACKET_SIZE];
    short len;
    float x;
    float y;

    buf += packet_decode (buf, "sff", &len, name, &x, &y);
    map_explosion_create (map, name, x, y);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_blast_create)
{
    float x;
    float y;
    float rad;
    long damage;

    buf += packet_decode (buf, "fffl", &x, &y, &rad, &damage);
    map_blast_create (map, x, y, rad, damage, OBJID_CLIENT_PROCESSED);

    return buf;
}


SC_GAMEINFO_HANDLER (sc_sound_play)
{
    float x;
    float y;
    char sample_name[NETWORK_MAX_PACKET_SIZE];
    short len;
    SAMPLE *spl;

    buf += packet_decode (buf, "ffs", &x, &y, &len, &sample_name);
    spl = store_get_dat (sample_name);
    if (spl)
	sound_play_once (spl, x, y);
    return buf;
}


static void process_sc_gameinfo_packet (const uchar_t *buf, size_t size)
{
    const void *end = buf + size;

    dbg ("process gameinfo packet");

    while (buf != end) {

	switch (*buf++) {

	    case MSG_SC_GAMEINFO_MAPLOAD:
		buf = sc_mapload (buf);
		break;

	    case MSG_SC_GAMEINFO_CLIENT_AIM_ANGLE:
		buf = sc_client_aim_angle (buf);
		break;

	    case MSG_SC_GAMEINFO_CLIENT_STATUS:
		buf = sc_client_status (buf);
		break;

	    case MSG_SC_GAMEINFO_CLIENT_SCORE:
		buf = sc_client_score (buf);
		break;

	    case MSG_SC_GAMEINFO_OBJECT_CREATE:
		buf = sc_object_create (buf);
		break;

	    case MSG_SC_GAMEINFO_OBJECT_DESTROY:
		buf = sc_object_destroy (buf);
		break;

	    case MSG_SC_GAMEINFO_OBJECT_UPDATE:
		buf = sc_object_update (buf);
		break;

	    case MSG_SC_GAMEINFO_OBJECT_HIDDEN:
		buf = sc_object_hidden (buf);
		break;

	    case MSG_SC_GAMEINFO_OBJECT_CALL:
		buf = sc_object_call (buf);
		break;

	    case MSG_SC_GAMEINFO_PARTICLES_CREATE:
		buf = sc_particles_create (buf);
		break;

	    case MSG_SC_GAMEINFO_BLOD_CREATE:
		buf = sc_blod_create (buf);
		break;

	    case MSG_SC_GAMEINFO_EXPLOSION_CREATE:
		buf = sc_explosion_create (buf);
		break;

	    case MSG_SC_GAMEINFO_BLAST_CREATE:
		buf = sc_blast_create (buf);
		break;

	    case MSG_SC_GAMEINFO_SOUND_PLAY:
		buf = sc_sound_play (buf);
		break;

	    default:
		error ("error: unknown code in gameinfo packet (client)\n");
	}
    }

    dbg ("done process gameinfo packet");
}



/*
 *----------------------------------------------------------------------
 *	Send text message
 *----------------------------------------------------------------------
 */


void client_send_text_message (const char *text)
{
    net_send_rdm_encode (conn, "cs", MSG_CS_TEXT, text);
}



/*
 *----------------------------------------------------------------------
 *	Lua bindings
 *----------------------------------------------------------------------
 */


void client_spawn_explosion (const char *name, float x, float y)
{
    map_explosion_create (map, name, x, y);
}


void client_play_sound (object_t *obj, const char *sample)
{
    SAMPLE *spl = store_get_dat (sample);

    if (spl)
	sound_play_once (spl, object_x (obj), object_y (obj));
}


void client_set_camera (int pushable, int max_dist)
{
    camera_set (cam, pushable, max_dist);
}



/*
 *----------------------------------------------------------------------
 *	Camera stuff
 *----------------------------------------------------------------------
 */


static int update_camera (void)
{
    int oldx, oldy;

    if (backgrounded)
	return 0;

    if (!tracked_object)
	return 0;

    oldx = camera_x (cam);
    oldy = camera_y (cam);

    camera_track_object_with_mouse (cam, tracked_object, mouse_x, mouse_y);

    return (oldx != camera_x (cam)) || (oldy != camera_y (cam));
}



/*
 *----------------------------------------------------------------------
 *	Draw stuff to screen
 *----------------------------------------------------------------------
 */


static void draw_status (BITMAP *bmp)
{
    FONT *f = store_get_dat ("/basic/font/ugly");
    int col = makecol24 (0xfb, 0xf8, 0xf8);

    text_mode (-1);
    textprintf_right_trans_magic (
	bmp, f, bmp->w/3 - 40, bmp->h - text_height (f) - 2, col,
	"%d", health);
    textprintf_right_trans_magic (
	bmp, f, bmp->w/3 - 2, bmp->h - text_height (f) - 2, col,
	"%d", ammo);
}


static void draw_scores (BITMAP *bmp)
{
    if (scores_brightness > 0) {
	FONT *f = store_get_dat ("/basic/font/ugly");
	int br = scores_brightness << 4;
	int col = makecol24 (br|0xb, br|0x8, br|0x8);
	int cx = bmp->w/3/2;

	textout_centre_trans_magic (bmp, f, "SCORES", cx, 20, col);

	{
	    client_info_t *c;
	    int th = text_height (f);
	    int y = (20 + th + 10);

	    list_for_each (c, &client_info_list) {
		textprintf_right_trans_magic (bmp, f, cx, y, col, "%s:", c->name);
		if (c->score)
		    textout_trans_magic (bmp, f, c->score, cx + 8, y, col);
		y += (text_height (f) + 5);
	    }
	}
    }
}
    

static void update_screen (void)
{
    if (backgrounded)
	return;

    clear_bitmap (bmp);

    if (bkgd) {
	int x = -(camera_x (cam) / (3*parallax_x));
	int y = -(camera_y (cam) / parallax_y);
	int w = bkgd->w;
	int h = bkgd->h;

	blit (bkgd, bmp, 0, 0, x, y, w, h);
	blit (bkgd, bmp, 0, 0, x, y+h, w, h);
	blit (bkgd, bmp, 0, 0, x+w, y, w, h);
	blit (bkgd, bmp, 0, 0, x+w, y+h, w, h);
    }

    render (bmp, map, cam);

    if (local_object) {
	int x = object_x (local_object) - camera_x (cam);
	int y = object_y (local_object) - camera_y (cam);

	aim_angle = atan2 (mouse_y - y, mouse_x - x);

	pivot_trans_magic_sprite (bmp, store_get_dat ("/basic/player/torch"),
				  x, y, 0, 115/2,
				  fatan2 (mouse_y - y, mouse_x - x));
    }

    if (crosshair) {
	draw_lit_magic_sprite (bmp, crosshair, mouse_x-2, mouse_y-2,
			       makecol24 (0xff, 0xff, 0xff));
    }

    draw_status (bmp);

    messages_render (bmp);

    text_mode (-1);
    textprintf_trans_magic (bmp, font, 0, 0, makecol24 (0x88, 0x88, 0xf8),
			    "%d FPS", fps);

    draw_scores (bmp);

    scare_mouse ();
    blit_magic_bitmap_to_screen (bmp);
    unscare_mouse ();

    frames++;
}



/*
 *----------------------------------------------------------------------
 *	The game client outer loop (XXX too big)
 *----------------------------------------------------------------------
 */


static void temporary_message (char *s, ...)
{
    va_list ap;
    int y = 0;

    va_start (ap, s);

    clear_bitmap (screen);

    do {
	textout (screen, font, s, 0, y, makecol (255, 255, 255));
	y += text_height (font);
    } while ((s = va_arg (ap, char *)));

    va_end (ap);
}


void client_run (int client_server)
{
    dbg ("connecting (state 1)");
    {
	int status;

	temporary_message ("Connecting to server...",
			   "Press Q to cancel", NULL);

	do {
	    if (key[KEY_ESC])
		return;
	    sync_client_lock ();
	    status = net_poll_connect (conn);
	    sync_client_unlock ();
	} while (!status);

	if (status < 1)
	    return;
    }

    dbg ("connecting (stage 2)");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];

	temporary_message ("Having a chat with the server...", NULL);

	while (1) {
	    sync_client_lock ();

	    if (net_receive_rdm (conn, buf, sizeof buf) <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_JOININFO:
		    packet_decode (buf+1, "l", &client_id);
		    net_send_rdm_encode (conn, "ccs", MSG_CS_JOININFO,
					 NETWORK_PROTOCOL_VERSION, client_name);
		    if (client_server)
			server_set_client_to_quit_with (client_id);
		    sync_client_unlock ();
		    goto lobby;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }

  lobby:

    dbg ("lobby");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];

	/* XXX */
	BITMAP *load_jpg(AL_CONST char *filename, RGB *pal);
	BITMAP *lobby_bmp;

	lobby_bmp = load_jpg ("data/lobby-tmp.jpg", 0);
	if (lobby_bmp) {
	    BITMAP *tmp = get_magic_bitmap_format (lobby_bmp, 0);
	    destroy_bitmap (lobby_bmp);
	    lobby_bmp = tmp;
	    set_magic_bitmap_brightness (lobby_bmp, 0xf, 0xf, 0xf);
	}
	/* end XXX */

	while (1) {
	    sync_client_lock ();

	    if (!messages_grabbed_keyboard ()) {
		if (key[KEY_ESC]) {
		    if (lobby_bmp) destroy_bitmap (lobby_bmp); /* XXX */
		    sync_client_unlock ();
		    goto disconnect;
		}
	    }

	    {
		const char *s = messages_poll_input ();

		if (s) {
		    if (client_server && (s[0] == ','))
			client_server_interface_add_input (s+1);
		    else
			client_send_text_message (s);
		}
	    }

	    if (!lobby_bmp)
		clear_bitmap (bmp);
	    else
		blit (lobby_bmp, bmp, 0, 0, 0, 0, bmp->w, bmp->h);
	    messages_render (bmp);
	    blit_magic_bitmap_to_screen (bmp);

	    if (net_receive_rdm (conn, buf, sizeof buf) <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMESTATEFEED_REQ:
		    if (lobby_bmp) destroy_bitmap (lobby_bmp); /* XXX */
		    sync_client_unlock ();
		    goto receive_game_state;

		case MSG_SC_CLIENT_ADD:
		    process_sc_client_add (buf+1);
		    break;

		case MSG_SC_CLIENT_REMOVE:
		    process_sc_client_remove (buf+1);
		    break;

		case MSG_SC_DISCONNECTED:
		    if (lobby_bmp) destroy_bitmap (lobby_bmp); /* XXX */
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }

  receive_game_state:

    dbg ("receive game state");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];
	size_t size;

	sync_client_lock ();
	net_send_rdm_byte (conn, MSG_CS_GAMESTATEFEED_ACK);
	sync_client_unlock ();

	while (1) {
	    sync_client_lock ();

	    if (key[KEY_ESC]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_GAMESTATEFEED_DONE:
		    dbg ("game state feed done");
		    sync_client_unlock ();
		    goto pause;

		case MSG_SC_CLIENT_ADD:
		    process_sc_client_add (buf+1);
		    break;

		case MSG_SC_CLIENT_REMOVE:
		    process_sc_client_remove (buf+1);
		    break;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }

  pause:

    dbg ("pause");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];
	size_t size;

	while (1) {
	    sync_client_lock ();

	    if (key[KEY_ESC]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMESTATEFEED_REQ:
		    sync_client_unlock ();
		    goto receive_game_state;

		case MSG_SC_CLIENT_ADD:
		    process_sc_client_add (buf+1);
		    break;

		case MSG_SC_CLIENT_REMOVE:
		    process_sc_client_remove (buf+1);
		    break;

		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_RESUME:
		    sync_client_unlock ();
		    goto game;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }

  game:

    dbg ("game");
    {
	ulong_t last_ticks, t;

	/* good time to force gc */
	lua_setgcthreshold (Lclt, 0);

	ticks_init ();
	last_ticks = ticks;

	pinging = 0;
	last_ping_time = 0;

	while (1) {
	    sync_client_lock ();

	    if (key[KEY_ESC]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    dbg ("process network input");
	    {
		uchar_t buf[NETWORK_MAX_PACKET_SIZE];
		size_t size;
		int receive_game_state_later = 0;
		int pause_later = 0;
		int lobby_later = 0;
		int end_later = 0;

		while ((size = net_receive_rdm (conn, buf, sizeof buf)) > 0) {
		    switch (buf[0]) {
			case MSG_SC_GAMESTATEFEED_REQ:
			    receive_game_state_later = 1;
			    break;

			case MSG_SC_CLIENT_ADD:
			    process_sc_client_add (buf+1);
			    break;

			case MSG_SC_CLIENT_REMOVE:
			    process_sc_client_remove (buf+1);
			    break;

			case MSG_SC_PAUSE:
			    pause_later = 1;
			    break;

			case MSG_SC_RESUME:
			    pause_later = 0;
			    break;

			case MSG_SC_GAMEINFO:
			    process_sc_gameinfo_packet (buf+1, size-1);
			    break;

			case MSG_SC_LOBBY:
			    lobby_later = 1;
			    break;

			case MSG_SC_TEXT: {
			    char string[NETWORK_MAX_PACKET_SIZE];
			    short len;

			    packet_decode (buf+1, "s", &len, string);
			    messages_add ("%s", string);
			    break;
			}

			case MSG_SC_PONG:
			    pinging = 0;
			    lag = (ticks - last_ping_time) / 2;
			    net_send_rdm_byte (conn, MSG_CS_BOING);
			    break;

			case MSG_SC_DISCONNECTED:
			    end_later = 1;
			    break;
		    }
		}

		if (receive_game_state_later) { sync_client_unlock (); goto receive_game_state; }
		if (pause_later) { sync_client_unlock (); goto pause; }
		if (lobby_later) { sync_client_unlock (); goto lobby; }
		if (end_later) { sync_client_unlock (); goto end; }
	    }

	    t = ticks;
	    if (last_ticks != t) {
		dbg ("send gameinfo");
		send_gameinfo_controls ();
		send_gameinfo_weapon_switch ();

		/* XXX dunno where to put this */
		if (!messages_grabbed_keyboard ()) {
		    int want_scores = key[KEY_TAB];
		    scores_brightness += (want_scores ? 1 : -1);
		    scores_brightness = MID (0, scores_brightness, 15);
		}

		/* Hack: this makes Allegro half-heartedly grab the
		 * mouse inside a window. */
		{int x,y; get_mouse_mickeys (&x, &y);}

		dbg ("do physics");
		perform_simple_physics (t, t - last_ticks);

		dbg ("poll update hooks");
		poll_update_hooks ((t - last_ticks) * MSECS_PER_TICK);

		map_destroy_stale_objects (map);

		{
		    const char *s;

		    if ((s = messages_poll_input ())) {
			if (client_server && (s[0] == ','))
			    client_server_interface_add_input (s+1);
			else
			    client_send_text_message (s);
		    }
		}

		dbg ("update camera");
		{
		    int n = t - last_ticks;
		    while ((n--) && update_camera ());
		}

		dbg ("update screen");
		update_screen ();

		dbg ("update sound reference point");
		if (local_object)
		    sound_update_reference_point (object_x (local_object),
						  object_y (local_object));

		last_ticks = t;
	    }

	    dbg ("handling pinging");
	    if ((!pinging) && (ticks > last_ping_time + (2 * TICKS_PER_SECOND))) {
		pinging = 1;
		last_ping_time = ticks;
		net_send_rdm_byte (conn, MSG_CS_PING);
	    }

	    lua_setgcthreshold (Lclt, 0);

	    sync_client_unlock ();
	}

	ticks_shutdown ();
    }

  disconnect:

    dbg ("disconnect");
    {
	timeout_t timeout;
	uchar_t c;

	sync_client_lock ();
	net_send_rdm_byte (conn, MSG_CS_DISCONNECT_ASK);
	sync_client_unlock ();

	timeout_set (&timeout, 2000);

	while (!timeout_test (&timeout)) {
	    sync_client_lock ();
	    if (net_receive_rdm (conn, &c, 1) > 0)
		if (c == MSG_SC_DISCONNECTED) {
		    dbg ("server confirmed disconnect");
		    sync_client_unlock ();
		    break;
		}
	    sync_client_unlock ();
	}

	goto end;
    }

  end:

    dbg ("end");
}



/*
 *----------------------------------------------------------------------
 *	Initialisation and shutdown
 *----------------------------------------------------------------------
 */


int client_init (const char *name, int net_driver, const char *addr)
{
    if (!(conn = net_openconn (net_driver, NULL)))
	return -1;
    net_connect (conn, addr);

    client_name = ustrdup (name);

    bmp = create_magic_bitmap (screen_width, screen_height);
    cam = camera_create (screen_width, screen_height);

    /* XXX */ {
	PALETTE pal;
	BITMAP *tmp;

	bkgd = load_bitmap ("data/basic/basic-bkgd/fluorescence.jpg", pal);
	if (bkgd) {
	    tmp = get_magic_bitmap_format (bkgd, pal);
	    destroy_bitmap (bkgd);
	    bkgd = tmp;
	}
    }

    crosshair = store_get_dat ("/basic/crosshair/000");

    map = NULL;
    local_object = NULL;
    tracked_object = NULL;

    client_info_list_init ();

    fps_init ();

    display_switch_init ();

    last_controls = 0;

    return 0;
}


void client_shutdown (void)
{
    display_switch_shutdown ();
    fps_shutdown ();
    client_info_list_free ();
    if (map) {
	map_destroy (map);
	map = NULL;
    }
    destroy_bitmap (bkgd);
    camera_destroy (cam);
    destroy_bitmap (bmp);
    free (client_name);
    net_closeconn (conn);
}
