#ifndef __included_fe_options_h
#define __included_fe_options_h


void options_menu_run (void);
int options_menu_init (BITMAP *background);
void options_menu_shutdown (void);

void load_config (int *desired_stretch_method);
void save_config (void);


#endif
