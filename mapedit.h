#ifndef _included_mapedit_h_
#define _included_mapedit_h_

#define LCLICK	1
#define RCLICK  2
#define MCLICK  4

typedef struct editmode {
    
    void (*click)(int x, int y, int b);
    void (*draw)(int x, int y);
    
    void (*palette_key)(void);
    void (*palette_click)(int x, int y, int b);
    void (*palette_draw)(void);
    
} editmode_t;


extern BITMAP *dbuf;

extern int top, left;
extern int palette_x;
extern int screen_w, screen_h;


#endif
