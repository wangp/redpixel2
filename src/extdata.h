#ifndef __included_extdata_h
#define __included_extdata_h


struct DATAFILE;


void register_extended_datafile (void);
struct DATAFILE *load_extended_datafile (const char *filename);


#endif
