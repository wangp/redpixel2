#ifndef __included_gamesrv_h
#define __included_gamesrv_h


struct object;


typedef struct {
    void(*init) ();
    void (*shutdown) ();
    void (*add_log) (const char *prefix, const char *text);
    void (*set_status) (const char *text);
    const char *(*poll) ();
} game_server_interface_t;


int game_server_init (game_server_interface_t *, int net_driver);
void game_server_enable_single_hack ();
void game_server_run ();
void game_server_shutdown ();


struct object *game_server_spawn_object (const char *typename, float x, float y);
struct object *game_server_spawn_projectile (const char *typename, struct object *, float speed, float delta_angle);
void game_server_spawn_blood (float x, float y, long nparticles, float spread);
void game_server_spawn_sparks (float x, float y, long nparticles, float spread);
void game_server_spawn_blod (float x, float y, long nparticles);
void game_server_call_method_on_clients (struct object *, const char *method, const char *arg);


#endif
