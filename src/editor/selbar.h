#ifndef __included_selbar_h
#define __included_selbar_h


#include "edselect.h"


void selectbar_install (int x, int y, int w, int h);
void selectbar_uninstall ();
void selectbar_set_list (ed_select_list_t *list);
void selectbar_set_icon_size (int w, int h);
void selectbar_set_change_set_proc (void (*_left) (), void (*_right) ());
void selectbar_set_selected_proc (void (*proc) ());
void selectbar_set_top (int);
int selectbar_top ();
void selectbar_set_selected (int);
int selectbar_selected ();
char *selectbar_selected_name ();


#endif
