/* wrapper.h */

#ifndef _included_wrapper_h_
#define _included_wrapper_h_

/* $-start-export-block */

DATAFILE *df_get_item(DATAFILE *dat, char *name);   	
DATAFILE *df_item(DATAFILE *dat, unsigned int i);   	
void *df_dat(DATAFILE *d);	       			
int df_count(DATAFILE *dat, int type); 			
int df_foreach(DATAFILE *dat, int (*callback)(DATAFILE *d));

unsigned char *bmp_line(BITMAP *bmp, unsigned int row);	
int bmp_w(BITMAP *b);		
int bmp_h(BITMAP *b);	

/* $-end-export-block  */

#endif
