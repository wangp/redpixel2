/* read_a_line.c -- read a line from a stream, properly
 *
 * Peter Wang <tjaden@users.sourceforge.net>  (2002-09-01)
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ral.h"

/* Reads a line of characters from a stream and returns a pointer to a
   dynamically allocated string, which the caller must free.  Lines
   are delimited by either a newline character (\n) or the EOF.

   The returned string is guaranteed to be NUL-terminated.  Note that
   NUL characters can appear in the middle of the string (this is a
   feature).


   HANDLE_NEWLINE specifies what to do with the delimiting newline
   character (if present):

   - RL_TRIM: the delimiting character is discarded.

   - RL_PEEK: the delimiting character is not included in the output
     string, but is pushed back onto the stream.

   - RL_CONCAT: the delimiting character is included at the end of the
     output string.


   RET_LEN can be NULL, or a pointer to a size_t, in which case the
   length of the returned string will be stored there.  The extra
   terminating NUL character is not included in the count.

   RET_DELIM can be NULL, or a pointer to an int, in which case the
   delimiting character will be stored there (either a \n or EOF).


   If the function reaches the end of file before a single character
   is read, NULL is returned.

   If the function runs out of memory during its operation, NULL is
   returned.  {Rethink this; how to differentiate between OOM and
   EOF-before-any-char-read?}


   This interface was based off the READ-LINE procedure from Scsh,
   hammered to fit within the constraints of C.  Most of the beauty
   was lost in the process -- sorry about that.
*/

unsigned char *
read_a_line(FILE *fp,
	    enum RAL_HANDLE_NEWLINE handle_newline,
	    size_t *ret_len,
	    int *ret_delim)
{
    unsigned char *buf = NULL;
    size_t bufsize = 0;
    size_t i = 0;

    assert(fp != NULL);
    assert(handle_newline <= RAL_CONCAT);

    while (1) {
	int c = fgetc(fp);

	/* stop on EOF */
	if (c == EOF) {
	    if (ret_delim) *ret_delim = c;
	    break;
	}

	/* stop on newline, 'trim' mode */
	if ((c == '\n') && (handle_newline == RAL_TRIM)) {
	    if (ret_delim) *ret_delim = c;
	    break;
	}

	/* stop on newline, 'peek' mode */
	if ((c == '\n') && (handle_newline == RAL_PEEK)) {
	    ungetc(c, fp);
	    if (ret_delim) *ret_delim = c;
	    break;
	}

	/* resize the buffer as necessary */
	if (i+1 >= bufsize) {
	    unsigned char *tmp;

	    if (bufsize == 0)
		bufsize = 2;
	    else
		bufsize *= 2;

	    tmp = realloc(buf, bufsize);
	    if (!tmp) {  /* OOM */
		free(buf);
		return NULL;
	    }
	    buf = tmp;
	}

	/* insert the char */
	buf[i++] = c;

	/* stop on newline, 'concat' mode */
	if ((c == '\n') && (handle_newline == RAL_CONCAT)) {
	    if (ret_delim) *ret_delim = c;
	    break;
	}
    }

    if (buf) {
	/* try to save some memory */
	if (i+1 != bufsize) {
	    unsigned char *tmp = realloc(buf, i+1);
	    if (tmp)
		buf = tmp;
	}

	/* NUL-terminate */
	buf[i] = '\0';
    }

    /* done */
    if (ret_len) *ret_len = i;
    return buf;
}
