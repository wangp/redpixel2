#ifndef _included_mapedit_h_
#define _included_mapedit_h_

extern int mx, my;

extern BITMAP *editbuf;
extern int editbuf_w, editbuf_h;

extern BITMAP *palbuf;
extern int palbuf_w, palbuf_h;
extern int palbuf_update;

void register_mode(char *name, int switchkey, lua_Object table);

#endif
