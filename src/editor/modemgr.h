#ifndef __included_modemgr_h
#define __included_modemgr_h


struct editmode {
    char *name;
    void (*enter_mode) ();
    void (*leave_mode) ();
};


void modemgr_init ();
void modemgr_shutdown ();
void modemgr_register (struct editmode *mode);
void modemgr_select (char *name);


#endif
