#ifndef __included_server_h
#define __included_server_h


struct server_interface;


int server_init (struct server_interface *, int net_driver);
void server_enable_single_hack (void);
void server_run (void);
void server_shutdown (void);


#endif
