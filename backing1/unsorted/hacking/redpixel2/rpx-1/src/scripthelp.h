/* script.h */
#ifndef _included_script_h_
#define _included_script_h_

int __script_help__key_shifts(int flag);

void *__script_help__df(DATAFILE *d, int i);
int __script_help__df_item_num(DATAFILE *dat, char *name);
void *__script_help__df_item(DATAFILE *dat, char *name);
int __script_help__df_count(DATAFILE *dat, int type);

unsigned char *__script_help__bmp_line(BITMAP *bmp, unsigned int row);
int __script_help__bmp_w(BITMAP *b);
int __script_help__bmp_h(BITMAP *b);

unsigned short __script_help__get_tile(int x, int y);
void __script_help__set_tile(int x, int y, int t);

BITMAP *__script_help__BITMAP(void *d);

#endif
