#ifndef __included_read_a_line_h
#define __included_read_a_line_h

#include <stdio.h>

enum RAL_HANDLE_NEWLINE { RAL_TRIM, RAL_PEEK, RAL_CONCAT };

unsigned char *
read_a_line(FILE *fp,
	    enum RAL_HANDLE_NEWLINE handle_newline,
	    size_t *ret_len,
	    int *ret_delim);

#endif
