#ifndef __included_messages_h
#define __included_messages_h


struct BITMAP;


int messages_init (void);
void messages_shutdown (void);
void messages_render (struct BITMAP *);
void messages_add (const char *fmt, ...);
void messages_poll_input (void);


#endif
