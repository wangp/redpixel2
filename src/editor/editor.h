#ifndef __included_editor_h
#define __included_editor_h


extern struct map *editor_map;


int editor_init (void);
void editor_shutdown (void);
int editor_run (void);


#endif
