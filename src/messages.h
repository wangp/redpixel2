#ifndef __included_messages_h
#define __included_messages_h


struct BITMAP;


int messages_init (void);
void messages_shutdown (void);
void messages_add (const char *fmt, ...);
int messages_num_lines (void);
const char *messages_get_line (int i);

void ingame_messages_render (struct BITMAP *);
const char *ingame_messages_poll_input (void);
int ingame_messages_grabbed_keyboard (void);


#endif
