/* bindhelp.h : some things have problems binding into Lua.
 *  thats what this file is for...
 */

/* you have to use key(n) instead of key[n] */
char __ugly_but_true__key(int k) { return key[k]; }

/* access the dat field of a DATAFILE with this */
void *__ugly_but_true__dat(DATAFILE *d, int i) { return d[i].dat; }

/* casting */
BITMAP *__ugly_but_true__BITMAP(void *d) { return (BITMAP *)d; }
