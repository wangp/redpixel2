/* comm.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <libnet.h>
#include "alloc.h"
#include "comm.h"
#include "commmsg.h"
#include "list.h"


/* shortcuts in this file */
typedef struct srvclt srvclt_t;
typedef struct comm_server srv_t;
typedef struct comm_client clt_t;


int comm_init ()
{
    net_init ();
    net_loadconfig (NULL);
    return 0;
}


void comm_shutdown ()
{
    net_shutdown ();
}


/* Common to server and client.  */


static int net_send_rdm_with_type (NET_CONN *conn, int type, void *buffer, int size)
{
    char tmp[size+1];
    tmp[0] = (type & 0xff);
    if (size > 0)
	memcpy (tmp+1, buffer, size);
    return net_send_rdm (conn, tmp, size+1);
}


/* Server.  */


/* node in server's list of clients */
struct srvclt {
    struct srvclt *next;
    struct srvclt *prev;
    NET_CONN *conn;
    int awaiting_input;
};


static srvclt_t *srvclt_create (NET_CONN *conn)
{
    srvclt_t *clt;
    clt = alloc (sizeof *clt);
    clt->conn = conn;
    return clt;
}


static void srvclt_destroy (srvclt_t *srvclt)
{
    if (srvclt) {
	net_closeconn (srvclt->conn);
	free (srvclt);
    }
}


struct comm_server {
    NET_CONN *listen;
    struct list_head clients;
    char buffer[NET_MAX_PACKET_SIZE];
};


srv_t *comm_server_init ()
{
    NET_CONN *listen;
    srv_t *srv;
    
    if (!(listen = net_openconn (NET_DRIVER_SOCKETS, "")))
	return NULL;
    net_listen (listen);

    srv = alloc (sizeof *srv);
    srv->listen = listen;
    list_init (srv->clients);
    return srv;
}


void comm_server_shutdown (srv_t *srv)
{
    if (srv) {
	net_closeconn (srv->listen);
	list_free (srv->clients, srvclt_destroy);
	free (srv);
    }
}


int comm_server_check_new_clients (srv_t *srv)
{
    NET_CONN *conn;
    srvclt_t *srvclt;

    if (!(conn = net_poll_listen (srv->listen)))
	return 0;

    srvclt = srvclt_create (conn);
    list_add (srv->clients, srvclt);
    return 1;
}


int comm_server_count_clients (srv_t *srv)
{
    srvclt_t *clt;
    int n = 0;

    list_for_each (clt, &srv->clients) n++;
    return n;
}


void comm_server_broadcast (srv_t *srv, int type, void *buffer, int size)
{
    srvclt_t *clt;

    list_for_each (clt, &srv->clients)
	net_send_rdm_with_type (clt->conn, type, buffer, size);
}


void comm_server_broadcast_tick (srv_t *srv)
{
    comm_server_broadcast (srv, XMSG_SC_TICK, NULL, 0);
}


void comm_server_reset_input (srv_t *srv)
{
    srvclt_t *clt;
    
    list_for_each (clt, &srv->clients)
	clt->awaiting_input = 1;
}


int comm_server_poll_input (srv_t *srv, void (*callback) (int, void *, int))
{
    srvclt_t *clt;
    char buf[NET_MAX_PACKET_SIZE];
    int size;
    int waiting = 0;

    list_for_each (clt, &srv->clients) {
	if (clt->awaiting_input) {
	    waiting++;
	    
	    while ((size = net_receive_rdm (clt->conn, buf, sizeof buf)) > 0) {
		if (buf[0] == XMSG_CS_OVER) {
		    clt->awaiting_input = 0;
		    waiting--;
		    break;
		}

		callback (buf[0], buf+1, size-1);
	    }
	}
    }
    
    return !waiting;
}


/* Client.  */
			   
			   
struct comm_client {
    NET_CONN *conn;
};


clt_t *comm_client_init ()
{
    NET_CONN *conn;
    clt_t *clt;
    
    if (!(conn = net_openconn (NET_DRIVER_SOCKETS, NULL)))
	return NULL;
    
    clt = alloc (sizeof *clt);
    clt->conn = conn;
    return clt;
}


void comm_client_shutdown (clt_t *clt)
{
    if (clt) {
	net_closeconn (clt->conn);
	free (clt);
    }
}


int comm_client_connect (clt_t *clt, const char *addr)
{
    return net_connect (clt->conn, addr);
}


int comm_client_poll_connect (clt_t *clt)
{
    return net_poll_connect (clt->conn);
}


int comm_client_send (clt_t *clt, int type, void *buffer, int size)
{
    return net_send_rdm_with_type (clt->conn, type, buffer, size);
}


int comm_client_send_over (clt_t *clt)
{
    return net_send_rdm_with_type (clt->conn, XMSG_CS_OVER, NULL, 0);
}


int comm_client_poll (clt_t *clt, void (*callback) (int, void *, int))
{
    char buf[NET_MAX_PACKET_SIZE];
    int size;
    
    while ((size = net_receive_rdm (clt->conn, buf, sizeof buf)) > 0) {
	if (buf[0] == XMSG_SC_TICK)
	    return 1;
	callback (buf[0], buf+1, size-1);
    }
    
    return 0;
}
