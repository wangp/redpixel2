#ifndef __included_timeval_h
#define __included_timeval_h

#ifdef TARGET_WINDOWS

struct timeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday (struct timeval *tp, void *unused_tz);

#else

#include <sys/time.h>

#endif

#endif
