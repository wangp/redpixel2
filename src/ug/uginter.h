#ifndef __included_uginter_h
#define __included_uginter_h


/* ugwidget.c */

struct ug_widget_class {
    int (*create) (ug_widget_t *, void *);
    void (*destroy) (ug_widget_t *);
    void (*event) (ug_widget_t *, int, ug_event_t *);
};
  

struct ug_widget {
    ug_widget_class_t *class;	/* class type */
    void *private;		/* class private data */
    ug_dialog_t *dialog;	/* parent dialog */
    char *id;

    int rw, rh;			/* requested size */
    int x, y, w, h;		/* allocated pos, size */
    void (*slot) (ug_widget_t *, int, void *);
};


/* ugdialog.c */

struct ug_dialog {
    void *window;		/* parent window */
    int border;			/* margin between widgets (for re-layout) */

    int num;
    ug_widget_t **widget;
    ug_widget_t *focus, *hasmouse;
};


#endif
