#ifndef LIST_H
#define LIST_H

typedef long   indx_t;
typedef double elem_t;

struct Node
{
    indx_t prev = 0;
    indx_t next = 0;
};

struct List 
{
    indx_t capacity = 0;

    elem_t* data_arr = nullptr;

    Node* node_arr = nullptr;
    Node* free_arr = nullptr;
};

enum List_err
{
    LIST_NOERR     =  0,
    LIST_NULLPTR   = -1,
    LIST_BAD_ALLOC = -2,

};

List_err list_init(List* list, elem_t* data = nullptr, indx_t data_cap = 0);

void     list_dstr(List* list);

indx_t   list_push_front(List* list, elem_t  elem);

indx_t   list_push_back(List* list, elem_t  elem);

indx_t   list_pop_front(List* list, elem_t* elem);

indx_t   list_pop_back(List* list, elem_t* elem);

indx_t   list_insert(List* list, indx_t pos, elem_t elem);

indx_t   list_extract(List* list, indx_t pos, elem_t* elem);

#endif // LIST_H