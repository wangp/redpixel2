#ifndef __included_modemgr_h
#define __included_modemgr_h


struct editmode {
    char *name;
    void (*enter_mode) (void);
    void (*leave_mode) (void);
};


void modemgr_init (void);
void modemgr_shutdown (void);
void modemgr_register (struct editmode *mode);
void modemgr_select (char *name);


#endif
