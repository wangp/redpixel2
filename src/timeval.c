/* timeval.c - struct timeval replacement for those without it
 *
 * This was adapted from lib/timeval.c from the Curl library.
 * The original copyright message is in the comment block below.
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */

/*
 * Copyright (C) 2000, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * In order to be useful for every potential user, curl and libcurl are
 * dual-licensed under the MPL and the MIT/X-derivate licenses.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the MPL or the MIT/X-derivate
 * licenses. You may pick one of these licenses.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 */


#include <time.h>
#include "timeval.h"


#ifdef TARGET_WINDOWS

#include <winalleg.h>

int gettimeofday (struct timeval *tp, void *unused_tz)
{
    SYSTEMTIME st;
    time_t tt;
    struct tm tmtm;
    /* mktime converts local to UTC */
    GetLocalTime (&st);
    tmtm.tm_sec = st.wSecond;
    tmtm.tm_min = st.wMinute;
    tmtm.tm_hour = st.wHour;
    tmtm.tm_mday = st.wDay;
    tmtm.tm_mon = st.wMonth - 1;
    tmtm.tm_year = st.wYear - 1900;
    tmtm.tm_isdst = -1;
    tt = mktime (&tmtm);
    tp->tv_sec = tt;
    tp->tv_usec = st.wMilliseconds * 1000;
    return 1;
}

#endif
