#ifndef __included_modes_h
#define __included_modes_h


int mode_tiles_init (void);
void mode_tiles_shutdown (void);


int mode_lights_init (void);
void mode_lights_shutdown (void);
void mode_lights_toggle (void);


int mode_objects_init (void);
void mode_objects_shutdown (void);


int mode_starts_init (void);
void mode_starts_shutdown (void);


#endif
