#ifndef __included_timeout_h
#define __included_timeout_h


#include <sys/time.h>


typedef struct {
    struct timeval timeout;
} timeout_t;


void timeout_set (timeout_t *, unsigned long msec);
int timeout_test (timeout_t *);


#endif
