#ifndef __included_sync_h
#define __included_sync_h


void sync_init (void *(*server_thread)(void *));
void sync_shutdown ();
void sync_server_lock ();
void sync_server_unlock ();
void sync_client_lock ();
void sync_client_unlock ();


#endif
