#ifndef __included_client_h
#define __included_client_h


struct object;


int client_init (const char *name, int net_driver, const char *addr);
void client_run (int client_server);
void client_shutdown (void);

void client_send_text_message (const char *text);

void client_play_sound (struct object *, const char *sample); /* Lua binding */
void client_set_camera (int pushable, int max_dist); /* Lua binding (internal) */


#endif
