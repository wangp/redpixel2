/* netclnt.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "net.h"
#include "netinter.h"


static NET_CONN *conn;
static int state;
static int timestamp;
static unsigned char packet[NET_MAX_PACKET_SIZE];
static void (*message_processor) (const unsigned char *packet, int size);


int net_client_init (int type, const char *target,
		     void (*proc) (const unsigned char *packet, int size))
{
    state = STATE_DISCONNECTED;

    conn = net_openconn (type, 0);
    if (!conn) 
	return -1;

    if (net_connect (conn, target)) {
	net_closeconn (conn);
	return -1;
    }

    message_processor = proc;

    state = STATE_CONNECTING;
    timestamp = net_time;
    return 0;
}


void net_client_shutdown ()
{
    state = STATE_DISCONNECTED;

    if (conn) {
	net_closeconn (conn);
	conn = 0;
    }
}


static int read_packet ()
{
    packet[0] = PACKET_NONE;
    return net_receive_rdm (conn, packet, sizeof packet);
}


static void process_packet (const unsigned char *packet, int size)
{
    switch (packet[0]) {
	case PACKET_MESSAGE:
  	    if (message_processor)
		message_processor (packet + 1, size - 1);
	    break;

	case PACKET_DISCONNECT:
	    state = STATE_DISCONNECT;
	    break;

	case PACKET_BADCOMMAND:
	    break;

	case PACKET_PING: {
	    unsigned char packet[1] = { PACKET_PONG };
	    net_send_rdm (conn, packet, 1);
	    break;
	}

	case PACKET_PONG:
	    break;
    }
}


int net_client_poll ()
{
    switch (state) {
	case STATE_CONNECTED:
	    if (net_query_rdm (conn)) {
		int size = read_packet ();
		if (size > 0)
		    process_packet (packet, size);
	    }
	    break;

	case STATE_DISCONNECT:
	    packet[0] = PACKET_DISCONNECT;
	    net_send_rdm (conn, packet, 1);
	    timestamp = net_time;
	    state = STATE_DISCONNECTING;
	    break;

	case STATE_DISCONNECTING:
	    if (net_query_rdm (conn)) {
		read_packet ();
		if (packet[0] == PACKET_DISCONNECT)
		    state = STATE_DISCONNECTED;
	    }
	    break;

	case STATE_FAILURE:
	case STATE_DISCONNECTED:
	    return -1;
	
	case STATE_CONNECTING: {
	    if ((net_time - timestamp) > TIME_CONNECT_PERIOD) {
		int x = net_poll_connect (conn);
		if (x < 0)
		    state = STATE_FAILURE;
		else if (x > 0)
		    state = STATE_REGISTER;
		timestamp = net_time;
	    }
	    break;
	}

	case STATE_REGISTER:
	    packet[0] = PACKET_REGISTER;
	    ustrcpy (packet + 1, "hello");
	    net_send_rdm (conn, packet, 1 + ustrlen (packet + 1));
	    state = STATE_CONNECTED;
	    break;
    }

    return 0;
}


void net_client_disconnect ()
{
    if (state == STATE_CONNECTED)
	state = STATE_DISCONNECT;
    else
	state = STATE_DISCONNECTED;
}


void net_client_send (const unsigned char *packet, int size)
{
    if (state == STATE_CONNECTED) {
	unsigned char buf[size + 1];
	buf[0] = PACKET_MESSAGE;
	memcpy (buf + 1, packet, size);
	net_send_rdm (conn, buf, size + 1);
    }
}
