/* commmsg.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include "commmsg.h"


long msg_get_long (const unsigned char *buf)
{
    return (((long) buf[0] << 24) |
	    ((long) buf[1] << 16) |
	    ((long) buf[2] <<  8) |
	    ((long) buf[3] <<  0));
}


void msg_put_long (unsigned char *buf, long l)
{
    buf[0] = (int)((l & 0xFF000000L) >> 24);
    buf[1] = (int)((l & 0x00FF0000L) >> 16);
    buf[2] = (int)((l & 0x0000FF00L) >>  8);
    buf[3] = (int)((l & 0x000000FFL) >>  0);
}


float msg_get_float (const unsigned char *buf)
{
    float f;
    memcpy (&f, buf, sizeof (float));
    return f;
}


void msg_put_float (unsigned char *buf, float f)
{
    memcpy (buf, &f, sizeof (float));
}
