#ifndef __included_svstats_h
#define __included_svstats_h


extern int svstats_incoming_bytes;
extern int svstats_outgoing_bytes;
extern float svstats_average_incoming_bytes;
extern float svstats_average_outgoing_bytes;

void svstats_init (void);
int svstats_poll (void);
void svstats_shutdown (void);


#endif
