#ifndef __included_resource_h
#define __included_resource_h


void resource_init ();
void resource_shutdown ();
void add_resource_path (char *path);
char *find_resource (char *dest, char *filename, int size);


#endif
