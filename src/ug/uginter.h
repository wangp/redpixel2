#ifndef __included_uginter_h
#define __included_uginter_h


/* ugwidget.c */

struct ug_widget_class {
    int (*create) (ug_widget_t *, void *);
    void (*destroy) (ug_widget_t *);
    void (*event) (ug_widget_t *, ug_event_t, ug_event_data_t *);
};
  

struct ug_widget {
    ug_widget_class_t *class;	/* class type */
    void *private;		/* class private data */
    ug_dialog_t *dialog;	/* parent dialog */
    char *id;

    int rw, rh;			/* requested size */
    int x, y, w, h;		/* allocated pos, size */
    void (*slot) (ug_widget_t *, ug_signal_t, void *);
};

ug_widget_t *ug_widget_create (ug_widget_class_t *, void *data, const char *id);
void ug_widget_destroy (ug_widget_t *);


#endif
