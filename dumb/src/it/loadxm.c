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
 * loadxm.c - Code to read a Fast Tracker II        / / \  \
 *            file, opening and closing it for     | <  /   \_
 *            you.                                 |  \/ /\   /
 *                                                  \_  /  > /
 * By entheh.                                         | \ / /
 *                                                    |  ' /
 *                                                     \__/
 */

#include "dumb.h"
#include "internal/it.h"



/* dumb_load_xm(): loads an XM file into a DUH struct, returning a pointer
 * to the DUH struct. When you have finished with it, you must pass the
 * pointer to unload_duh() so that the memory can be freed.
 */
DUH *dumb_load_xm(const char *filename)
{
	DUH *duh;
	DUMBFILE *f = dumbfile_open(filename);

	if (!f)
		return NULL;

	duh = dumb_read_xm(f);

	dumbfile_close(f);

	return duh;
}