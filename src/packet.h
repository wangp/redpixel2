#ifndef __included_packet_h
#define __included_packet_h


int packet_encode (unsigned char *buf, const char *fmt, ...);
int packet_encode_v (unsigned char *buf, const char *fmt, va_list ap);
int packet_decode (const unsigned char *buf, const char *fmt, ...);


#endif
