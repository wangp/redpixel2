#ifndef __included_net_h
#define __included_net_h


int net_main_init ();
void net_main_shutdown ();


int net_client_init (int type, const char *target, void (*proc) (const unsigned char *packet, int size));
void net_client_shutdown ();
int net_client_poll ();
void net_client_disconnect ();
void net_client_send (const unsigned char *packet, int size);


int net_server_init (void (*proc)(int id, const unsigned char *packet, int size));
void net_server_shutdown ();
int net_server_poll ();
void net_server_ask_quit ();


#endif
