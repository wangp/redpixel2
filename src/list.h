#ifndef __included_list_h
#define __included_list_h


/* Linked list code, inspired by Links, but written from scratch.  */

typedef struct list_head list_node_t;

struct list_head {
    void *next;
    void *prev;
};

#define list_init(L)		do { L.next = L.prev = &L; } while (0)

#define list_empty(L)		(L.next == &L)

#define list_add_at_pos(L, n)				\
do {							\
    list_node_t *LL = (void *) L, *nn = (void *) n;	\
    nn->next = LL->next;				\
    ((list_node_t *) nn->next) -> prev = nn;		\
    nn->prev = (void *) LL;				\
    LL->next = nn;					\
} while (0)

#define list_add(L, n)		list_add_at_pos ((&L), n)

#define list_append(L, n)				\
do {							\
    list_node_t *LL = (void *) &L, *nn = (void *) n;	\
    nn->prev = LL->prev;				\
    ((list_node_t *) nn->prev) -> next = nn;		\
    nn->next = (void *) LL;				\
    LL->prev = nn;					\
} while (0)
    
#define list_remove(n)					\
do {							\
    list_node_t *nn = (void *) n;			\
    ((list_node_t *) nn->prev) -> next = nn->next;	\
    ((list_node_t *) nn->next) -> prev = nn->prev;	\
    nn->next = nn->prev = 0;				\
} while (0)

#define list_for_each(n, L)				\
for (n = (L)->next; n != (void *) (L); n = ((list_node_t *) n) -> next)
    
#define list_for_each_back(n, L)			\
for (n = (L)->prev; n != (void *) (L); n = ((list_node_t *) n) -> prev)

#define list_free(L, dtor)			\
do {						\
    list_node_t *x, *y;				\
    for (x = L.next; x != (void *) &L; x = y) {	\
	y = x->next;				\
	dtor ((void *) x);			\
    }						\
    list_init (L);				\
} while (0)


#endif
