#ifndef __included_list_h
#define __included_list_h


/* Linked list code, inspired by Links, but written from scratch.  */

struct list_head {
    void *next;
    void *prev;
};

#define init_list(L)		do { L.next = L.prev = &L; } while (0)
#define list_empty(L)		(L.next == &L)
#define add_at_pos(L, n)	do { n->next = L->next; n->next->prev = n; n->prev = (void *) L; L->next = n; } while (0)
#define add_to_list(L, n)	add_at_pos ((&L), n)
#define append_to_list(L, n)	do { n->prev = L.prev; n->prev->next = n; n->next = (void *) &L; L.prev = n; } while (0)
#define del_from_list(n)	do { n->prev->next = n->next; n->next->prev = n->prev; } while (0)
#define foreach(n, L)		for (n = L.next; n != (void *) &L; n = n->next)
#define free_list(L, dtor)	do { struct list_head *x, *y; for (x = L.next; x != (void *) &L; x = y) { y = x->next; dtor ((void *) x); } init_list (L); } while (0)


#endif
