/* packet.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifndef TEST
# include "packet.h"
# include "error.h"
#else
# include <stdio.h>
# define error(msg)  puts(msg)
#endif


static long get_long (const unsigned char *buf)
{
    return (((long) buf[0] << 24) |
	    ((long) buf[1] << 16) |
	    ((long) buf[2] <<  8) |
	    ((long) buf[3] <<  0));
}


static void put_long (unsigned char *buf, long l)
{
    buf[0] = (int)((l & 0xFF000000L) >> 24);
    buf[1] = (int)((l & 0x00FF0000L) >> 16);
    buf[2] = (int)((l & 0x0000FF00L) >>  8);
    buf[3] = (int)((l & 0x000000FFL) >>  0);
}


static float get_float (const unsigned char *buf)
{
    float f;
    memcpy (&f, buf, sizeof (float));
    return f;
}


static void put_float (unsigned char *buf, float f)
{
    memcpy (buf, &f, sizeof (float));
}


int packet_encode (unsigned char *buf, const char *fmt, ...)
{
    va_list ap;
    int size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);

    return size;
}


int packet_encode_v (unsigned char *buf, const char *fmt, va_list ap)
{
    unsigned char *orig = buf;
    
    for (; *fmt; fmt++) switch (*fmt) {

	case 'c':
	    /* chars are promoted to ints in variadics */
	    *buf++ = va_arg (ap, int);
	    break;

	case 'l':
	    put_long (buf, va_arg (ap, long)); buf += 4;
	    break;

	case 'f':
	    /* floats are promoted to doubles in variadics */
	    put_float (buf, va_arg (ap, double)); buf += 4;
	    break;

	case 's': {
	    const char *str;
	    long len;
		
	    str = va_arg (ap, const char *);
	    len = strlen (str);
	    put_long (buf, len); buf += 4;
	    memcpy (buf, str, len); buf += len;
	    break;
	}

	default:
	    error ("internal error: unrecognised format specifier in packet_encode\n");
	    break;
    }

    return buf - orig;
}


int packet_decode (const unsigned char *buf, const char *fmt, ...)
{
    va_list ap;
    const unsigned char *orig = buf;

    va_start (ap, fmt);

    for (; *fmt; fmt++) switch (*fmt) {

	case 'c':
	    *(va_arg (ap, char *)) = *buf++;
	    break;

	case 'l':
	    *(va_arg (ap, long *)) = get_long (buf); buf += 4;
	    break;
		
	case 'f':
	    *(va_arg (ap, float *)) = get_float (buf); buf += 4;
	    break;

	case 's': {
	    /* XXX possible buffer overflow */
	    long *len;
	    char *s;

	    len = va_arg (ap, long *);
	    *len = get_long (buf); buf += 4;
	    s = va_arg (ap, char *);
	    memcpy (s, buf, *len);
	    s[*len] = '\0';
	    buf += *len;
	    break;
	}

	default:
	    error ("internal error: unrecognised format specifier in packet_decode\n");
	    break;
    }

    va_end (ap);

    return buf - orig;
}



/* tester */

#ifdef TEST

int main ()
{
    char buf[512];
    long a, b, c;
    float f;
    long len;
    char s[80];

    packet_encode (buf, "lflsl", 1, 101.101, -1, "[my string]", 0xbeefcafe);
    packet_decode (buf, "lflsl", &a, &f, &b, &len, &s, &c);
    printf ("The following should be equal:\n"
	    "%ld == 1\n"
	    "%f == 101.101\n"
	    "%ld == -1\n"
	    "%s == [my string]  (%ld == 11)\n"
	    "0x%lx == 0xbeefcafe\n",
	    a, f, b, s, len, c);
    return 0;
}

#endif
