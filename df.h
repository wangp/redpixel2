#ifndef _included_df_h_
#define _included_df_h_

int df_count(DATAFILE *dat, int type);
int df_foreach(DATAFILE *dat, int (*callback)(DATAFILE *d));

#endif
