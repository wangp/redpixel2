#ifndef __included_mylibnet_h
#define __included_mylibnet_h


struct NET_CONN;


int net_send_rdm_byte (struct NET_CONN *, unsigned char);
int net_send_rdm_encode (struct NET_CONN *, const char *fmt, ...);


#endif
