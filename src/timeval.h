#ifndef __included_timeval_h
#define __included_timeval_h

#ifdef TARGET_WINDOWS

struct pwtimeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday (struct pwtimeval *tp, void *unused_tz);

#else

#include <sys/time.h>

#define pwtimeval timeval

#endif

#endif
