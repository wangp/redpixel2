#ifndef __included_explo_h
#define __included_explo_h


struct BITMAP;


typedef struct explosion explosion_t;


void explosion_init (void);
void explosion_shutdown (void);
int explosion_type_register (const char *name, const char *first_frame,
			     int nframes, int tics, const char *light,
			     const char *sound);
explosion_t *explosion_create (const char *name, int x, int y);
void explosion_destroy (explosion_t *e);
int explosion_update (explosion_t *e);
void explosion_draw (struct BITMAP *dest, explosion_t *explo,
		     int offset_x, int offset_y);
void explosion_draw_lights (struct BITMAP *dest, explosion_t *explo,
			    int offset_x, int offset_y);


#endif
