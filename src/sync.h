#ifndef __included_sync_h
#define __included_sync_h


void sync_init (void *(*server_thread)(void *));
void sync_shutdown (void);
void sync_server_lock (void);
void sync_server_unlock (void);
int sync_server_stop_requested (void);
void sync_client_lock (void);
void sync_client_unlock (void);


#endif
