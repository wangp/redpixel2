#ifndef __included_fe_widgets_h
#define __included_fe_widgets_h


#define hline_w(b, x, y, w, c)		hline (b, x, y, x + w - 1, c)
#define rect_wh(b, x, y, w, h, c)	rect (b, x, y, x + w - 1, y + h - 1, c)
#define rectfill_wh(b, x, y, w, h, c)	rectfill (b, x, y, x + w - 1, y + h - 1, c)
#define set_clip_wh(b, x, y, w, h)	set_clip (b, x, y, x + w - 1, y + h - 1)

extern FONT *fancy_edit_font;
extern FONT *fancy_font;
extern SAMPLE *type_sound;

int fancy_bitmap_proc(int msg, DIALOG *d, int c);
int fancy_label_proc (int msg, DIALOG *d, int c);
int fancy_button_proc (int msg, DIALOG *d, int c);
int fancy_checkbox_proc (int msg, DIALOG *d, int c);
int fancy_radio_proc (int msg, DIALOG *d, int c);
int fancy_edit_proc (int msg, DIALOG *d, int c);
int _fancy_list_proc (int msg, DIALOG *d, int c, FONT *font);
int fancy_textbox_proc (int msg, DIALOG *d, int c);
int fancy_slider_proc (int msg, DIALOG *d, int c);

void make_button_background (DIALOG *d, BITMAP *bmp, BITMAP *backdrop, int bg);
void set_fancy_label (DIALOG *d, int fg, FONT *font);
void create_fancy_button (DIALOG *d, BITMAP *backdrop, int fg, int bg);
void destroy_fancy_button (DIALOG *d);
void create_fancy_checkbox (DIALOG *d, BITMAP *backdrop, int fg, int bg, BITMAP *tick);
void destroy_fancy_checkbox (DIALOG *d);
void create_fancy_radio_button (DIALOG *d, BITMAP *backdrop, int fg, int bg);
void destroy_fancy_radio_button (DIALOG *d);
void create_fancy_editbox (DIALOG *d, BITMAP *backdrop, int fg, int bg);
void destroy_fancy_editbox (DIALOG *d);
void create_fancy_listbox (DIALOG *d, BITMAP *backdrop, int fg, int bg);
void destroy_fancy_listbox (DIALOG *d);
void create_fancy_slider (DIALOG *d, BITMAP *backdrop, int fg, int bg);
void destroy_fancy_slider (DIALOG *d);

extern BITMAP *fancy_screen;

#define D_FANCY_DIRTY (D_USER)

int fancy_gui_init (void);
void fancy_gui_shutdown (void);
int fancy_do_dialog (DIALOG *dialog, int focus_obj);
int blit_fancy_dirty_to_screen (DIALOG *dialog);

#endif
