#ifndef __included_selbar_h
#define __included_selbar_h


struct ed_select_list;


void selectbar_install (int x, int y, int w, int h);
void selectbar_uninstall (void);
void selectbar_set_list (struct ed_select_list *list);
void selectbar_set_icon_size (int w, int h);
void selectbar_set_change_set_proc (void (*_left) (void),
				    void (*_right) (void));
void selectbar_set_selected_proc (void (*proc) (void));
void selectbar_scroll_up (void);
void selectbar_scroll_down (void);
void selectbar_set_top (int);
int selectbar_top (void);
void selectbar_set_selected (int);
int selectbar_selected (void);
char *selectbar_selected_name (void);


#endif
