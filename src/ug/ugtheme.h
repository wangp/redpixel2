#ifndef __included_ugtheme_h
#define __included_ugtheme_h


#define UG_THEME_BG		0
#define UG_THEME_FG		1

#define UG_THEME_ARROW_UP	0
#define UG_THEME_ARROW_DOWN	1


struct ug_theme {
    int (*init) ();
    void (*shutdown) ();
    
    void (*tile) (struct BITMAP *, int, int, int, int, int);
    void (*bevel) (struct BITMAP *, int, int, int, int, int);
    void (*arrow) (struct BITMAP *, int, int, int, int, int);
    struct FONT *(*font) (int);
    void (*text) (struct BITMAP *, struct FONT *, int, int, const char *);
};

    
int ug_theme_init ();
void ug_theme_shutdown ();

void ug_theme_tile (struct BITMAP *, int x, int y, int w, int h, int type);
void ug_theme_bevel (struct BITMAP *, int x, int y, int w, int h, int invert);
void ug_theme_arrow (struct BITMAP *, int x, int y, int w, int h, int type);
void ug_theme_text_centre (struct BITMAP *, int x, int y, int w, int h, int type, const char *fmt, ...);

struct FONT *ug_theme_font (int type);


#endif
