#ifndef __included_rect_h
#define __included_rect_h

/* just sick of writing this */

#define in_rect(x, y, rx, ry, rw, rh)		\
	 (!(   ((x) < (rx))			\
	    || ((y) < (ry))			\
	    || ((x) >= (rx) + (rw))		\
	    || ((y) >= (ry) + (rh))))

#endif
