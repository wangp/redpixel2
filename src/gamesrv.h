#ifndef __included_gamesrv_h
#define __included_gamesrv_h



typedef struct {
    void(*init) ();
    void (*shutdown) ();
    void (*add_log) (const char *prefix, const char *text);
    const char *(*poll) ();
} game_server_interface_t;


int ng_game_server_init (game_server_interface_t *);
void ng_game_server ();
void ng_game_server_shutdown ();


#endif
