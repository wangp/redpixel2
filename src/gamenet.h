#ifndef __included_gamenet_h
#define __included_gamenet_h


void gamenet_server_process_message (int id, const unsigned char *packet, int size);
void gamenet_server_send_game_state ();
void gamenet_server_replicate_object_destroy (unsigned long id);
void gamenet_server_replicate_variable_change (unsigned long id, float x, float y, float xv, float yv);

void gamenet_client_process_message (const unsigned char *packet, int size);
void gamenet_client_receive_game_state ();


#endif
