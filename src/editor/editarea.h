#ifndef __included_editarea_h
#define __included_editarea_h


/* For no particular reason, these are the same as the gui.h numbers.  */

#define EDITAREA_EVENT_KEY_TYPE		4
#define EDITAREA_EVENT_MOUSE_MOVE	12
#define EDITAREA_EVENT_MOUSE_DOWN	13
#define EDITAREA_EVENT_MOUSE_UP		14


struct editarea_event {
    int offx, offy;
    
    struct {
	int x, y, b, bstate;
    } mouse;
    
    struct {
	int key;
    } key;
};


void editarea_install (int x, int y, int w, int h);
void editarea_uninstall ();
void editarea_reset_offset ();

void editarea_layer_register (const char *name,
			      void (*draw) (struct BITMAP *, int offx, int offy),
			      int (*event) (int event, struct editarea_event *),
			      int depth);
void editarea_layer_show (const char *name, int show);
void editarea_layer_activate (const char *name);


#endif
