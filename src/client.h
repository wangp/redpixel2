#ifndef __included_client_h
#define __included_client_h


int client_init (const char *name, int net_driver, const char *addr);
void client_run (void);
void client_shutdown (void);

/* lua bindings */
void client_set_camera (int pushable, int max_dist);


#endif
