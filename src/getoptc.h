#ifndef __included_getoptc_h
#define __included_getoptc_h


#ifdef HAVE_GETOPT

# include <unistd.h>

#else

extern char *optarg;
extern int   optind;
extern int   opterr;
extern int   optopt;

int getopt(int argc, char *argv[], char *opstring);

#endif


#endif
