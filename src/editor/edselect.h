#ifndef __included_edselect_h
#define __included_edselect_h


#include "ug.h"

struct BITMAP;


typedef struct ed_select_list ed_select_list_t;

ed_select_list_t *ed_select_list_create ();
void ed_select_list_destroy (ed_select_list_t *);
void ed_select_list_add_item (ed_select_list_t *, const char *name, struct BITMAP *);
char *ed_select_list_item_name (ed_select_list_t *, int);
int ed_select_list_item_index (ed_select_list_t *, const char *name);


#define ED_SELECT_SIGNAL_SELECTED	-1

extern ug_widget_class_t ed_select;

void ed_select_set_list (ug_widget_t *, ed_select_list_t *);
void ed_select_set_icon_size (ug_widget_t *, int w, int h);
void ed_select_scroll_up (ug_widget_t *);
void ed_select_scroll_down (ug_widget_t *);
void ed_select_scroll_page_up (ug_widget_t *);
void ed_select_scroll_page_down (ug_widget_t *);

void ed_select_set_top (ug_widget_t *, int);
int ed_select_top (ug_widget_t *);
void ed_select_set_selected (ug_widget_t *, int);
int ed_select_selected (ug_widget_t *);


#endif
