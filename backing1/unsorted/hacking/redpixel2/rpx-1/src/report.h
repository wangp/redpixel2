#ifndef _included_report_h_
#define _included_report_h_

#include <stdio.h>

/* will be removed out of non debug version */
#define debugprintf printf

/* for the console system that I will put in later */
#ifdef Xwin_ALLEGRO
#define consoleprintf printf
#else
#define consoleprintf printf
#endif

/* for printing error messages */
void errprintf(char *fmt, ...);

#endif
