#ifndef _included_report_h_
#define _included_report_h_

/* will be removed out of non debug version */
#define debugprintf printf

#ifdef Xwin_ALLEGRO
#define consoleprintf printf
#else

#endif

void errprintf(char *fmt, ...);

#endif
