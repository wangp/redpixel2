#ifndef _included_script_h_
#define _included_script_h_

#define scLazy(x)	(scAdd_External_Symbol((#x), (x)))

void script_error(void);

int exec_script(char *filename, char *func);

#endif
