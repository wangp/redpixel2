#ifndef __included_list_h
#define __included_list_h


/* Linked list code, inspired by Links, but written from scratch.  */

struct list_head {
    void *next;
    void *prev;
/*
    struct list_head *next;
    struct list_head *prev;
 */
};

#define init_list(L)		do { L.next = L.prev = &L; } while (0)
#define list_empty(L)		(L.next == &L)
#define add_at_pos(L, n)	do { struct list_head *LL = (void *) L, *nn = (void *) n; nn->next = LL->next; ((struct list_head *)nn->next)->prev = nn; nn->prev = (void *) LL; LL->next = nn; } while (0)
#define add_to_list(L, n)	add_at_pos ((&L), n)
#define append_to_list(L, n)	do { struct list_head *LL = (void *) &L, *nn = (void *) n; nn->prev = LL->prev; ((struct list_head *)nn->prev)->next = nn; nn->next = (void *) LL; LL->prev = nn; } while (0)
#define del_from_list(n)	do { n->prev->next = n->next; n->next->prev = n->prev; } while (0)
#define foreach(n, L)		for (n = (void *) L.next; n != (void *) &L; n = n->next)
#define foreachback(n, L)	for (n = (void *) L.prev; n != (void *) &L; n = n->prev)
#define free_list(L, dtor)	do { struct list_head *x, *y; for (x = L.next; x != (void *) &L; x = y) { y = x->next; dtor ((void *) x); } init_list (L); } while (0)


#endif
