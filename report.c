/* Error reporting functions */

#include <stdio.h>
#include <stdarg.h>
#include <allegro.h>

#ifdef Xwin_ALLEGRO

void errprintf(char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    
    printf("Error: %s", buf);
}

#else				       



#endif
