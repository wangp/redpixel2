#ifndef __included_console_h
#define __included_console_h


int console_init ();
void console_shutdown ();
void console_open ();
void console_close ();
void console_printf (const char *fmt, ...);
void console_draw (BITMAP *buf);
void console_update ();


#endif
