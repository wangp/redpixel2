#ifndef __included_ticks_h
#define __included_ticks_h


/*
 * Note that "ticks" and "tics" are different!  "Ticks" are a measure
 * of game time.  "Tics" are used in the Lua code as counters of some
 * other unit of time (usually multiples of the update hook's speed).
 */

#define TICKS_PER_SECOND	(50)
#define MSECS_PER_TICK		(1000 / TICKS_PER_SECOND)


#endif
