#ifndef __included_fe_main_h
#define __included_fe_main_h


extern int client_was_kicked;
extern int desired_game_screen_w, desired_game_screen_h;
extern int desired_menu_screen_w, desired_menu_screen_h;

int set_menu_gfx_mode (void);
void set_menu_mouse_range (void);

void begin_connecting_dialog (void);
int poll_connecting_dialog (void);
void end_connecting_dialog (void);

void resize_dialog (DIALOG *d);
int initialize_fancy_dialog (DIALOG *d, BITMAP *bmp, int fg, int bg);
void shutdown_fancy_dialog (DIALOG *d);

void gamemenu_run (void);
int gamemenu_init (void);
void gamemenu_shutdown (void);


#endif
