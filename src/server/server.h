#ifndef __included_server_h
#define __included_server_h


#include "id.h"

struct server_interface;


int server_init (struct server_interface *, int net_driver);
void server_inhibit_double_message (void);
void server_set_client_to_quit_with (client_id_t id);
void server_run (void);
void server_shutdown (void);


#endif
