#ifndef __included_gameclt_h
#define __included_gameclt_h


int game_client_init (const char *name, int net_driver, const char *addr);
void game_client_run ();
void game_client_shutdown ();


#endif
