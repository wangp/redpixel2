#ifndef _included_convtbl_h_
#define _included_convtbl_h_

typedef struct convitem
{
    char *key;
    void *data;
} convitem_t;

typedef struct convtable
{
    unsigned int size;
    convitem_t *tbl;
} convtable_t;


struct convtable *construct_convtable(unsigned int size);
void destroy_convtable(struct convtable *tbl, void (*freer)(void *));
convitem_t *add_convtable_item(convtable_t *tbl, char *name, void *data);


#endif
