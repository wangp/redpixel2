/* svclient.c - the server's list of clients
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include <allegro.h>
#include "libnet.h"
#include "alloc.h"
#include "list.h"
#include "netmsg.h"
#include "packet.h"
#include "server.h"
#include "svclient.h"
#include "svgame.h"
#include "svintern.h"
#include "svstats.h"


list_head_t svclients;
static int svclients_next_id = 1;



/*
 *----------------------------------------------------------------------
 *	Creating, destroying, modifying
 *----------------------------------------------------------------------
 */


svclient_t *svclient_create (NET_CONN *conn)
{
    svclient_t *c = alloc (sizeof *c);
    c->conn = conn;
    c->id = svclients_next_id++;
    c->name = ustrdup ("(unknown)");
    list_append (svclients, c);
    return c;
}


void svclient_destroy (svclient_t *c)
{
    if (c) {
	list_remove (c);
	net_closeconn (c->conn);
	string_free (c->score);
	string_free (c->name);
	free (c);
    }
}


void svclient_set_name (svclient_t *c, const char *name)
{
    string_set (c->name, name);
}


void svclient_set_score (svclient_t *c, const char *score)
{
    string_set (c->score, score);
}


void svclient_set_timeout (svclient_t *c, int secs)
{
    timeout_set (&c->timeout, secs * 1000);
}


int svclient_timed_out (svclient_t *c)
{
    return svclient_cantimeout (c) && timeout_test (&c->timeout);
}



/*
 *----------------------------------------------------------------------
 *	Network functions
 *----------------------------------------------------------------------
 */


int svclient_send_rdm (svclient_t *c, const void *buf, size_t size)
{
    int x = net_send_rdm (c->conn, buf, size);
    if (x == 0)
	svstats_outgoing_bytes += size;
    else if (x < 0) {
	svclient_set_state (c, SVCLIENT_STATE_BITOFF);
	server_log ("Client %s was disconnected [send error]", c->name);
    }
    return x;
}


int svclient_send_rdm_byte (svclient_t *c, char byte)
{
    return svclient_send_rdm (c, &byte, 1);
}


int svclient_send_rdm_encode (svclient_t *c, const char *fmt, ...)
{
    va_list ap;
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);
    return svclient_send_rdm (c, buf, size);
}


int svclient_receive_rdm (svclient_t *c, void *buf, size_t size)
{
    int n;
    if (!net_query_rdm (c->conn))
	return 0;
    if ((n = net_receive_rdm (c->conn, buf, size)) > 0)
	svstats_incoming_bytes += n;
    return n;
}



/*
 *----------------------------------------------------------------------
 *	Network broadcast functions
 *----------------------------------------------------------------------
 */


void svclients_broadcast_rdm (const void *buf, size_t size)
{
    svclient_t *c;

    for_each_svclient (c)
	if ((c->state != SVCLIENT_STATE_BITOFF) ||
	    (c->state != SVCLIENT_STATE_STALE))
	    svclient_send_rdm (c, buf, size);
}


void svclients_broadcast_rdm_byte (char c)
{
    svclients_broadcast_rdm (&c, 1);
}


void svclients_broadcast_rdm_encode (const char *fmt, ...)
{
    va_list ap;
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    svclients_broadcast_rdm (buf, size);
    va_end (ap);
}



/*
 *----------------------------------------------------------------------
 *	Clients list
 *----------------------------------------------------------------------
 */


int svclients_count (void)
{
    svclient_t *c;
    int n = 0;
    for_each_svclient (c) n++;
    return n;
}


svclient_t *svclients_find_by_id (int id)
{
    svclient_t *c;

    for_each_svclient (c)
	if (c->id == id) return c;

    return NULL;
}


svclient_t *svclients_find_by_name (const char *name)
{
    svclient_t *c;

    for_each_svclient (c)
	if (!ustricmp (c->name, name)) return c;

    return NULL;
}


void svclients_remove_stale (void)
{
    svclient_t *c, *next;
    
    for (c = svclients.next; list_neq (c, &svclients); c = next) {
	next = list_next (c);
	if (c->state == SVCLIENT_STATE_STALE)
	    svclient_destroy (c);
    }
}



/*
 *----------------------------------------------------------------------
 *	Module
 *----------------------------------------------------------------------
 */


void svclients_init (void)
{
    list_init (svclients);
    svclients_next_id = 1;
}


void svclients_shutdown (void)
{
    list_free (svclients, svclient_destroy);
}
