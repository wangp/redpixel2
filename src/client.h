#ifndef __included_client_h
#define __included_client_h


#include "id.h"


struct object;


typedef struct client_info {
    struct client_info *next;
    struct client_info *prev;
    client_id_t id;
    char *name;
    char *face_icon;
    char *score;
} client_info_t;


int client_init (const char *name, int net_driver, const char *addr);
void client_run (int client_server);
void client_shutdown (void);

int client_num_clients (void);
client_info_t *client_get_nth_client_info (int n);

void client_send_text_message (const char *text);

/* begin Lua bindings */
void client_spawn_explosion (const char *name, float x, float y);
void client_play_sound (struct object *, const char *sample);
void client_set_camera (int pushable, int max_dist); /* internal */
/* end Lua bindings */


#endif
