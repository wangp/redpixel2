#ifndef __included_gamesrv_h
#define __included_gamesrv_h


struct object;


typedef struct {
    void(*init) ();
    void (*shutdown) ();
    void (*add_log) (const char *prefix, const char *text);
    const char *(*poll) ();
} game_server_interface_t;


int game_server_init (game_server_interface_t *, int net_driver);
void game_server_enable_single_hack ();
void game_server_run ();
void game_server_shutdown ();


int game_server_spawn_projectile (const char *typename, struct object *, float speed);


#endif
