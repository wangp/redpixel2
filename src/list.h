#ifndef __included_list_h
#define __included_list_h


/* Linked list code, inspired by Links, but written from scratch.  */

struct list_head {
    void *next;
    void *prev;
};

#define list_init(L)		do { L.next = L.prev = &L; } while (0)

#define list_empty(L)		(L.next == &L)

#define list_add_at_pos(L, n)					\
do {								\
    struct list_head *LL = (void *) L, *nn = (void *) n;	\
    nn->next = LL->next;					\
    ((struct list_head *) nn->next)->prev = nn;			\
    nn->prev = (void *) LL;					\
    LL->next = nn;						\
} while (0)

#define list_add(L, n)		list_add_at_pos ((&L), n)

#define list_append(L, n)					\
do {								\
    struct list_head *LL = (void *) &L, *nn = (void *) n;	\
    nn->prev = LL->prev;					\
    ((struct list_head *) nn->prev)->next = nn;			\
    nn->next = (void *) LL;					\
    LL->prev = nn;						\
} while (0)
    
#define list_remove(n)						\
do {								\
    n->prev->next = n->next;					\
    n->next->prev = n->prev;					\
} while (0)

#define list_for_each(n, L)					\
for (n = (void *) L.next; n != (void *) &L; n = n->next)
    
#define list_for_each_back(n, L)				\
for (n = (void *) L.prev; n != (void *) &L; n = n->prev)

#define list_free(L, dtor)					\
do {								\
    struct list_head *x, *y;					\
    for (x = L.next; x != (void *) &L; x = y) {			\
	y = x->next;						\
	dtor ((void *) x);					\
    }								\
    list_init (L);						\
} while (0)


#endif
