#ifndef __included_editarea_h
#define __included_editarea_h


#define EDITAREA_EVENT_MOUSE_MOVE	12
#define EDITAREA_EVENT_MOUSE_DOWN	13
#define EDITAREA_EVENT_MOUSE_UP		14


struct editarea_event {
    int offx, offy;
    
    struct {
	int x, y, b, bstate;
    } mouse;
};


void editarea_install (int x, int y, int w, int h);
void editarea_uninstall ();

void editarea_layer_register (const char *name, void (*draw) (BITMAP *, int offx, int offy),
			      int (*event) (int event, struct editarea_event *), int depth);
void editarea_layer_show (const char *name, int show);
void editarea_layer_activate (const char *name);


#endif
