#ifndef __included_list_h
#define __included_list_h


/* Welcome to void pointer country.  We trust you will not be stupid.
   Please leave all ZealotSoft(R) Type Safety Nag-o-Meters(tm) outside.  */


typedef struct list_head list_head_t;

struct list_head {
    void *next;
    void *prev;
};

#define list_init(L)		do { L.next = L.prev = &L; } while (0)
#define list_eq(m, n)		((void *) m == (void *) n)
#define list_neq(m, n)		(!(list_eq (m, n)))
#define list_empty(L)		(list_eq (L.next, &L))
#define list_next(n)		(((list_head_t *) n) -> next)
#define list_prev(n)		(((list_head_t *) n) -> prev)

#define list_add_at_pos(L, n)			\
do {						\
    void *LL = L, *nn = n;			\
    list_next (nn) = list_next (LL);		\
    list_prev (list_next (nn)) = nn;		\
    list_prev (nn) = LL;			\
    list_next (LL) = nn;			\
} while (0)

#define list_add(L, n)		list_add_at_pos ((&L), n)

#define list_append(L, n)			\
do {						\
    void *LL = &L, *nn = n;			\
    list_prev (nn) = list_prev (LL);		\
    list_next (list_prev (nn)) = nn;		\
    list_next (nn) = LL;			\
    list_prev (LL) = nn;			\
} while (0)
    							\
#define list_remove(n)					\
do {							\
    void *nn = n;					\
    list_next (list_prev (nn)) = list_next (nn);	\
    list_prev (list_next (nn)) = list_prev (nn);	\
    list_next (nn) = list_prev (nn) = 0;		\
} while (0)

#define list_for_each(n, L)				\
for (n = list_next (L); list_neq (n, L); n = list_next (n))

#define list_for_each_back(n, L)			\
for (n = list_prev (L); list_neq (n, L); n = list_prev (n))

#define list_free(L, dtor)			\
do {						\
    list_head_t *x, *y;				\
    for (x = L.next; list_neq (x, &L); x = y) {	\
	y = list_next (x);			\
	dtor ((void *) x);			\
    }						\
    list_init (L);				\
} while (0)


#endif
