#ifndef __included_messages_h
#define __included_messages_h


struct BITMAP;


int messages_init (void);
void messages_shutdown (void);
void messages_render (struct BITMAP *);
void messages_add (const char *fmt, ...);
const char *messages_poll_input (void);
int messages_grabbed_keyboard (void);


#endif
