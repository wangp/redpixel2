#ifndef __included_comm_h
#define __included_comm_h


int comm_init ();
void comm_shutdown ();


typedef struct comm_server comm_server_t;

comm_server_t *comm_server_init ();
void comm_server_shutdown (comm_server_t *);
int comm_server_check_new_clients (comm_server_t *);
int comm_server_count_clients (comm_server_t *);
void comm_server_broadcast (comm_server_t *, int type, void *buffer, int size);
void comm_server_broadcast_tick (comm_server_t *);
void comm_server_reset_input (comm_server_t *);
int comm_server_poll_input (comm_server_t *, void (*callback) (int, void *, int));
int comm_server_awaiting_input (comm_server_t *);


typedef struct comm_client comm_client_t;

comm_client_t *comm_client_init ();
void comm_client_shutdown (comm_client_t *);
int comm_client_connect (comm_client_t *, const char *addr);
int comm_client_poll_connect (comm_client_t *);
int comm_client_send (comm_client_t *, int msgtype, void *buffer, int size);
int comm_client_send_over (comm_client_t *clt);
int comm_client_poll (comm_client_t *, void (*callback) (int, void *, int));


#endif
