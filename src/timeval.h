/* XXX instead of emulating the gettimeofday interface, make our
 * own elapsed time interface */
#ifndef __included_timeval_h
#define __included_timeval_h

#ifdef TARGET_WINDOWS

struct pwtimeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday (struct pwtimeval *tp, void *unused_tz);
void gettimeofday_init (void);
void gettimeofday_shutdown (void);

#else

#include <sys/time.h>

#define pwtimeval timeval
#define gettimeofday_init()
#define gettimeofday_shutdown()

#endif

#endif
