/* mylibnet.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include "libnet.h"
#include "netmsg.h"
#include "packet.h"


int net_send_rdm_byte (NET_CONN *conn, unsigned char c)
{
    return net_send_rdm (conn, &c, 1);
}


int net_send_rdm_encode (NET_CONN *conn, const char *fmt, ...)
{
    unsigned char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;
    va_list ap;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);
    return net_send_rdm (conn, buf, size);
}
