/* gamenet.c - game network protocol
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "gameloop.h"		/* XXX: local_player */
#include "gamenet.h"
#include "map.h"
#include "net.h"
#include "object.h"
#include "yield.h"


/* Packet encoding and decoding helpers.  */

static unsigned char *write_char (unsigned char *p, int c)
{
    *p = c;
    return p + 1;
}

static unsigned char read_char (const unsigned char *p)
{
    return *p;
}

static const unsigned char *skip_char (const unsigned char *p)
{
    return p + 1;
}

static unsigned char *write_int (unsigned char *ptr, unsigned int i)
{
    ptr[0] = (i >> 24) & 0xff;
    ptr[1] = (i >> 16) & 0xff;
    ptr[2] = (i >>  8) & 0xff;
    ptr[3] = (i      ) & 0xff;
    return ptr + 4;
}

static unsigned int read_int (const unsigned char *ptr)
{
    return ((((((ptr[0] << 8) + ptr[1]) << 8) + ptr[2]) << 8) + ptr[3]);
}

static const unsigned char *skip_int (const unsigned char *p)
{
    return p + 4;
}

static unsigned char *write_float (unsigned char *p, float f)
{
    /* write floats as fixed (easier, but quantizes) */
    return write_int (p, ftofix (f));    
}

static float read_float (const unsigned char *p)
{
    return fixtof (read_int (p));
}

static const unsigned char *skip_float (const unsigned char *p)
{
    return skip_int (p);
}

static unsigned char *write_str (unsigned char *p, unsigned char *str)
{
    while (*str)
	*p++ = *str++;
    *p = '\0';
    return p + 1;
}

static const unsigned char *read_str (const unsigned char *p)
{
    return p;
}

static const unsigned char *skip_str (const unsigned char *p)
{
    while (*p) p++;
    return p + 1;
}
    

/*----------------------------------------------------------------------*/


#define GAMENET_REPLICATE_OBJECT_CREATE		0
#define GAMENET_REPLICATE_OBJECT_DESTROY	1
#define GAMENET_REPLICATE_VARIABLE_CHANGE	2
#define GAMENET_SET_LOCAL_PLAYER		3
#define GAMENET_GAME_START			4



/*
 * 	Server
 */


void gamenet_server_process_message (int id, const unsigned char *packet, int size)
{
    switch (packet[0]) {
    }    
}


static void send_replicate_object_create (object_t *o)
{
    unsigned char packet[NET_MAX_PACKET_SIZE];
    unsigned char *p = packet;

    p = write_char (p, GAMENET_REPLICATE_OBJECT_CREATE);
    p = write_int (p, o->id);
    p = write_str (p, o->type->name);
    p = write_float (p, o->x);
    p = write_float (p, o->y);
    p = write_float (p, o->xv);
    p = write_float (p, o->yv);

    net_server_broadcast (packet, p - packet);
}


void gamenet_server_send_game_state ()
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next) {
	send_replicate_object_create (p);
	yield ();
    }

    /* XXX: I don't think this should be here.  Also, num clients does
     * not imply num players, although once a lobby system is in we
     * can easily fix this.  */
    {
	int i;

	for (i = 0; i < net_server_num_clients; i++) {
	    object_t *o = object_create ("player", OBJECT_ROLE_AUTHORITY);
	    o->x = 8 * 16;	/* XXX */
	    o->y = 0 * 16;
	    map_link_object (map, o);
	    send_replicate_object_create (o);

	    {
		unsigned char packet[NET_MAX_PACKET_SIZE];
		unsigned char *p = packet;

		p = write_char (p, GAMENET_SET_LOCAL_PLAYER);
		p = write_int (p, o->id);
		net_server_broadcast (packet, p - packet);
	    }

	    local_player = o;	/* on server, for testing: XXX */
	}
    }
    
    {
	unsigned char packet[0] = { GAMENET_GAME_START };
    	net_server_broadcast (packet, 1);
    }
}


void gamenet_server_replicate_object_destroy (unsigned long id)
{
    unsigned char packet[NET_MAX_PACKET_SIZE];
    unsigned char *p = packet;

    p = write_char (p, GAMENET_REPLICATE_OBJECT_DESTROY);
    p = write_int (p, id);    
    net_server_broadcast (packet, p - packet);
}


void gamenet_server_replicate_variable_change (unsigned long id, float x, float y, float xv, float yv)
{
    unsigned char packet[NET_MAX_PACKET_SIZE];
    unsigned char *p = packet;

    p = write_char (p, GAMENET_REPLICATE_VARIABLE_CHANGE);
    p = write_int (p, id);
    p = write_float (p, x);
    p = write_float (p, y);
    p = write_float (p, xv);
    p = write_float (p, yv);

    net_server_broadcast (packet, p - packet);
}



/*
 *	Client 
 */


static int game_started;


static void handle_replicate_object_create (const unsigned char *packet)
{
    const unsigned char *p = packet;
    unsigned long id;
    object_t *obj;

    id = read_int (p);		
    obj = map_find_object (map, id);
    if (obj)	/* object already exists (probably server == client) */
	return;

    p = skip_int (p);
    obj = object_create (read_str (p), OBJECT_ROLE_PROXY);	p = skip_str (p);
    obj->x  = read_float (p);	p = skip_float (p);
    obj->y  = read_float (p);	p = skip_float (p);
    obj->xv = read_float (p);	p = skip_float (p);
    obj->yv = read_float (p);
    
    map_link_object (map, obj);
}


static void handle_replicate_object_destroy (const unsigned char *packet)
{
    unsigned long id; 
    object_t *obj;

    id = read_int (packet);
    obj = map_find_object (map, id);
    if (obj)
	obj->dying = 1;
}


static void handle_replicate_variable_change (const unsigned char *packet)
{
    const unsigned char *p = packet;
    unsigned long id; 
    object_t *obj;

    id = read_int (p);		p = skip_int (p);

    obj = map_find_object (map, id);
    if ((!obj) || (obj->role != OBJECT_ROLE_PROXY))
	return;

    obj->x = read_float (p);	p = skip_float (p);
    obj->y = read_float (p);	p = skip_float (p);
    obj->xv = read_float (p);	p = skip_float (p);
    obj->yv = read_float (p);	
}


static void handle_set_local_player (const unsigned char *packet)
{
    unsigned long id;

    id = read_int (packet);
    local_player = map_find_object (map, id);
}


void gamenet_client_process_message (const unsigned char *p, int size)
{
    switch (p[0]) {
	case GAMENET_REPLICATE_OBJECT_CREATE:
	    handle_replicate_object_create (p + 1);
	    break;
	case GAMENET_REPLICATE_OBJECT_DESTROY:
	    handle_replicate_object_destroy (p + 1);
	    break;
	case GAMENET_REPLICATE_VARIABLE_CHANGE:
	    handle_replicate_variable_change (p + 1);
	    break;	    
	case GAMENET_SET_LOCAL_PLAYER:
	    handle_set_local_player (p + 1);
	    break;
	case GAMENET_GAME_START:
	    game_started = 1;
	    break;
    }    
}


void gamenet_client_receive_game_state ()
{
    game_started = 0;

    while (!game_started) {
	net_client_poll ();
	yield ();
    }
}
