#ifndef __included_fe_lobby_h
#define __included_fe_lobby_h


void begin_client_server_lobby_dialog (void);
int poll_client_server_lobby_dialog (void);
void end_client_server_lobby_dialog (void);

void begin_client_lobby_dialog (void);
int poll_client_lobby_dialog (void);
void end_client_lobby_dialog (void);
const char *get_client_lobby_dialog_input (void);

int lobby_init (BITMAP *background);
void lobby_shutdown (void);


#endif
