/* df.c: datafile related functions */

#include <allegro.h>


/* df_count:
 *  Returns number of entries of TYPE (-1 = *) in datafile.
 */
int df_count(DATAFILE *dat, int type)
{
    DATAFILE *entry;
    int i = 0, c = 0;
    
    if (!dat)
      return 0;
    
    entry = &dat[0];
    while (entry->type != DAT_END)
    {
	if (type == -1 || dat->type == type)
	  c++;
	entry = &dat[++i];
    }
    
    return c;  
}


/* df_foreach:
 *  Runs a callback for each entry in a datafile.
 *  Returns non-zero on sucess, zero on error.
 */
int df_foreach(DATAFILE *dat, int (*callback)(DATAFILE *d))
{
    DATAFILE *entry;
    int i = 0;
    
    if (!dat)
      return 0;
    
    entry = &dat[0];
    while (entry->type != DAT_END)
    {
	if (callback(entry))
	  break;
	entry = &dat[++i];
    }
    
    return 1;
}

