#ifndef _included_script_h_
#define _included_script_h_

int exec_script(char *filename, char *func);   	/* $-export */

int add_script(char *filename);	       		/* $-export */
void free_scripts(void);	       	

#endif
