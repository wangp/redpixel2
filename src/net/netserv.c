/* netserv.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <string.h>
#include <allegro.h>
#include <libnet.h>
#include "net.h"
#include "netinter.h"


struct conn {
    NET_CONN *conn;
    int id;
    int state;
    int timestamp;
    char name[32];
    struct conn *next;
};


static int num_drivers;
static NET_CONN **listen;

static int id;
static struct conn clients;

static unsigned char packet[NET_MAX_PACKET_SIZE];
static void (*message_processor) (int id, const unsigned char *packet, int size);

static int quitting, quit_time;
static int running;


int net_server_init (void (*proc)(int id, const unsigned char *packet, int size))
{
    NET_DRIVERNAME *drv;
    int i;

    running = quitting = 0;

    for (num_drivers = 0, drv = net_drivers; drv->name; drv++)
	num_drivers++;
    if (!num_drivers)
	return -1;

    listen = malloc (sizeof (NET_CONN *) * num_drivers);
    if (!listen)
	return -1;

    for (i = 0, drv = net_drivers; drv->name; i++, drv++) {
	listen[i] = net_openconn (drv->num, "");
	if (listen[i]) {
	    if (net_listen (listen[i]))
		;		/* failed */
	    else
		;		/* listening */
	}
    }

    clients.next = 0;

    message_processor = proc;

    running = 1;
    return 0;
}


void net_server_shutdown ()
{
    int i;

    if (!running) return;
    running = quitting = 0;

    for (i = 0; i < num_drivers; i++)
	if (listen[i])
	    net_closeconn (listen[i]);
    
    free (listen);
	
    while (clients.next) {
	struct conn *p = clients.next;
	net_closeconn (p->conn);
	clients.next = p->next;
	free (p);
    }
}


static void poll_listen ()
{
    int i;
    NET_CONN *new;

    for (i = 0; i < num_drivers; i++) {
	if (!listen[i]) continue;

	new = net_poll_listen (listen[i]);
	if (new) {
	    struct conn *p = malloc (sizeof *p);

	    p->state = STATE_REGISTER;
	    p->conn = new;
	    p->timestamp = net_time;		

	    p->next = clients.next;
	    clients.next = p;
	}
    }
}


static void process_packet (struct conn *p, unsigned char *packet, int size)
{
    switch (packet[0]) {
	case PACKET_MESSAGE:
	    if (message_processor)
		message_processor (p->id, packet + 1, size - 1);
	    break;
	
	case PACKET_DISCONNECT:
	    net_send_rdm (p->conn, packet, 1);
	    p->timestamp = net_time;
	    p->state = STATE_DISCONNECTING;
	    break;

	case PACKET_COMMAND:
	    break;

	case PACKET_PING:
	    packet[0] = PACKET_PONG;
	    net_send_rdm (p->conn, packet, 1);
	    break;

	case PACKET_PONG:
	    break;
    }
}


static void poll_clients ()
{
    struct conn *p, *prev;

    for (prev = &clients, p = prev->next; p; prev = p, p = prev->next) {
	switch (p->state) {
	    case STATE_REGISTER:
		if ((net_time - p->timestamp) > TIME_SERVER_REGISTER) {
		    p->state = STATE_DISCONNECTED;
		}
		else if (net_receive_rdm (p->conn, packet, sizeof packet) > 0) {
		    if (packet[0] == PACKET_REGISTER) {
			p->id = id++;
			memset (p->name, 0, sizeof p->name);
			ustrncpy (p->name, packet + 1, sizeof p->name - 1);
			p->state = STATE_CONNECTED;
		    }
		    else {
			/*  bad packet */
		    }
		}
		break;

	    case STATE_CONNECTED:
		if (net_query_rdm (p->conn)) {
		    int size = net_receive_rdm (p->conn, packet, sizeof packet);
		    if (size > 0)
			process_packet (p, packet, size);
		}
		break;

	    case STATE_DISCONNECTING: {
		int out;
		net_conn_stats (p->conn, 0, &out);
		if (!out || (net_time - p->timestamp > TIME_SERVER_DISCONNECT))
		    p->state = STATE_DISCONNECTED;
		break;
	    }

	    case STATE_DISCONNECTED:
		net_closeconn (p->conn);
		prev->next = p->next;
		free (p);
		p = prev;
		break;
	}
    }
}


int net_server_poll ()
{
    if (quitting && (!clients.next || (net_time - quit_time > TIME_SERVER_SHUTDOWN)))
	return -1;

    if (running) {
	if (!quitting) poll_listen ();
	poll_clients ();
    }

    return 0;
}


void net_server_ask_quit ()
{
    struct conn *p;

    if (quitting) return;

    packet[0] = PACKET_DISCONNECT;
    for (p = clients.next; p; p = p->next)
	net_send_rdm (p->conn, packet, 1);

    quitting = 1;
    quit_time = net_time;
}


void net_server_broadcast (const unsigned char *packet, int size)
{
    unsigned char buf[size + 1];
    struct conn *p;

    buf[0] = PACKET_MESSAGE;
    memcpy (buf + 1, packet, size);

    for (p = clients.next; p; p = p->next)
	if (p->state == STATE_CONNECTED)
	    net_send_rdm (p->conn, buf, size + 1);
}
