/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /     '   '  '
 *  |  | \  \       |  |    ||         |   \/   |       .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |       '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |       .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/edicated  \____/niversal  /__\  /____\usic /|  .  . astardisation
 *                                                    /  \
 *                                                   / .  \
 * loads3m.c - Code to read a ScreamTracker 3       / / \  \
 *             file, opening and closing it for    | <  /   \_
 *             you.                                |  \/ /\   /
 *                                                  \_  /  > /
 * By entheh.                                         | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */

#include "dumb.h"
#include "internal/it.h"



/* dumb_load_s3m(): loads an S3M file into a DUH struct, returning a pointer
 * to the DUH struct. When you have finished with it, you must pass the
 * pointer to unload_duh() so that the memory can be freed.
 */
DUH *dumb_load_s3m(const char *filename)
{
	DUH *duh;
	DUMBFILE *f = dumbfile_open(filename);

	if (!f)
		return NULL;

	duh = dumb_read_s3m(f);

	dumbfile_close(f);

	return duh;
}
