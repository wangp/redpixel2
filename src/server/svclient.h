#ifndef __included_svclient_h
#define __included_svclient_h


#include <stdlib.h>
#include "id.h"
#include "timeout.h"


typedef struct svclient svclient_t;


typedef enum {
    SVCLIENT_STATE_JOINING,
    SVCLIENT_STATE_JOINED,
    SVCLIENT_STATE_BITOFF, /* almost stale: we need a chance to
			    broadcast that this client's game object
			    should be destroyed */
    SVCLIENT_STATE_STALE,  /* stale: will delete the client object */
} svclient_state_t;


struct svclient {
    svclient_t *next;
    svclient_t *prev;
    struct NET_CONN *conn;
    svclient_state_t state;
    int id;
    struct object *client_object;
    char *name;
    int flags;
    int controls;
    int old_controls;    
    float aim_angle;
    float last_sent_aim_angle;
    char *score;
    timeout_t timeout;
    unsigned long pong_time;
    int lag;
};


#define svclient_set_state(c,s)	((c)->state = (s))


#define SVCLIENT_FLAG_READY	0x01
#define SVCLIENT_FLAG_WANTFEED	0x02
#define SVCLIENT_FLAG_CANTIMEOUT	0x04

#define svclient_ready(c)             ((c)->flags &   SVCLIENT_FLAG_READY)
#define svclient_set_ready(c)         ((c)->flags |=  SVCLIENT_FLAG_READY)
#define svclient_clear_ready(c)       ((c)->flags &=~ SVCLIENT_FLAG_READY)
#define svclient_wantfeed(c)          ((c)->flags &   SVCLIENT_FLAG_WANTFEED)
#define svclient_set_wantfeed(c)      ((c)->flags |=  SVCLIENT_FLAG_WANTFEED)
#define svclient_clear_wantfeed(c)    ((c)->flags &=~ SVCLIENT_FLAG_WANTFEED)
#define svclient_cantimeout(c)        ((c)->flags &   SVCLIENT_FLAG_CANTIMEOUT)
#define svclient_set_cantimeout(c)    ((c)->flags |=  SVCLIENT_FLAG_CANTIMEOUT)
#define svclient_clear_cantimeout(c)  ((c)->flags &=~ SVCLIENT_FLAG_CANTIMEOUT)


extern struct list_head svclients;

#define for_each_svclient(c)  list_for_each(c, &svclients)


svclient_t *svclient_create (struct NET_CONN *conn);
void svclient_destroy (svclient_t *c);
void svclient_set_name (svclient_t *c, const char *name);
void svclient_set_score (svclient_t *c, const char *score);
void svclient_set_timeout (svclient_t *c, int secs);
int svclient_timed_out (svclient_t *c);

int svclient_send_rdm (svclient_t *c, const void *buf, size_t size);
int svclient_send_rdm_byte (svclient_t *c, char byte);
int svclient_send_rdm_encode (svclient_t *c, const char *fmt, ...);
int svclient_receive_rdm (svclient_t *c, void *buf, size_t size);

void svclients_broadcast_rdm (const void *buf, size_t size);
void svclients_broadcast_rdm_byte (char c);
void svclients_broadcast_rdm_encode (const char *fmt, ...);

int svclients_count (void);
svclient_t *svclients_find_by_id (client_id_t id);
svclient_t *svclients_find_by_name (const char *name);
void svclients_remove_stale (void);

void svclients_init (void);
void svclients_shutdown (void);


#endif
