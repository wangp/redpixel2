#ifndef __included_ugtheme_h
#define __included_ugtheme_h


#define UG_THEME_BG		0
#define UG_THEME_FG		1

#define UG_THEME_ARROW_UP	0
#define UG_THEME_ARROW_DOWN	1


struct ug_theme {
    int (*init) ();
    void (*shutdown) ();
    
    void (*tile) (BITMAP *, int, int, int, int, int);
    void (*bevel) (BITMAP *, int, int, int, int, int);
    void (*arrow) (BITMAP *, int, int, int, int, int);
    FONT *(*font) (int);
    void (*text) (BITMAP *, FONT *, int, int, const char *);
};

    
int ug_theme_init ();
void ug_theme_shutdown ();

void ug_theme_tile (BITMAP *, int x, int y, int w, int h, int type);
void ug_theme_bevel (BITMAP *, int x, int y, int w, int h, int invert);
void ug_theme_arrow (BITMAP *, int x, int y, int w, int h, int type);
void ug_theme_text_centre (BITMAP *, int x, int y, int w, int h, int type, const char *fmt, ...);


#endif
