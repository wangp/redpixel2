#ifndef __included_lobby_h
#define __included_lobby_h


int ask_host_and_name (char *host_buf, int host_buf_size,
		       char *name_buf, int name_buf_size);

void begin_client_connecting_dialog (void);
void force_redraw_client_connecting_dialog (void);
int poll_client_connecting_dialog (void);
void end_client_connecting_dialog (void);

void begin_client_lobby_dialog (void);
void force_redraw_client_lobby_dialog (void);
int poll_client_lobby_dialog (void);
void end_client_lobby_dialog (void);
const char *get_client_lobby_dialog_input (void);


#endif
