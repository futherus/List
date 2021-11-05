#include "List.h"
#include <stdio.h>
#include <stdlib.h>

#define DATA (list->data_arr)
#define NODS (list->node_arr)
#define FRES (list->free_arr)
#define TAIL (NODS[0].next)
#define HEAD (NODS[0].prev)
#define CAP  (list->capacity)

static inline indx_t list_free_(List* list)
{
    for(indx_t iter = 1; iter < list->capacity; iter++)
        if(NODS[iter].next == -1)
            return iter;
        
    return -1;
}

static inline indx_t list_insert_(List* list, indx_t pos, indx_t free, elem_t elem)
{
    if(!list || free == -1 || pos < 0)
        return -1;

    DATA[free] = elem;

    NODS[free].prev = pos;
    NODS[free].next = NODS[pos].next;

    NODS[NODS[pos].next].prev = free;
    NODS[pos].next = free;

    return free;
}

static inline indx_t list_extract_(List* list, indx_t pos, elem_t* elem)
{
    if(!list || !elem || pos <= 0)
        return -1;

    *elem = DATA[pos];

    NODS[NODS[pos].next].prev = NODS[pos].prev;
    NODS[NODS[pos].prev].next = NODS[pos].next;

    NODS[pos].prev = -1;
    NODS[pos].next = -1;

    return pos;
}

List_err list_init(List* list, elem_t* data, indx_t cap)
{
    if(!list || !data)
        return LIST_NULLPTR;

    DATA = data;
    CAP  = cap;

    NODS = (Node*) calloc(cap, sizeof(Node));
    FRES = (Node*) calloc(cap, sizeof(Node));

    if(!NODS || !FRES)
        return LIST_BAD_ALLOC;

    for(indx_t iter = 1; iter < CAP; iter++)
    {
        NODS[iter].next = -1;
        NODS[iter].prev = -1;
    }

    TAIL = 0;
    HEAD = 0;

    return LIST_NOERR;
}

void list_dstr(List* list)
{
    free(NODS);
}

indx_t list_insert(List* list, indx_t pos, elem_t elem)
{
    return list_insert_(list, pos, list_free_(list), elem);
}

indx_t list_extract(List* list, indx_t pos, elem_t* elem)
{
    return list_extract_(list, pos, elem);
}

indx_t list_push_back(List* list, elem_t elem)
{
    return list_insert_(list, 0, list_free_(list), elem);
}

indx_t list_push_front(List* list, elem_t elem)
{
    return list_insert_(list, HEAD, list_free_(list), elem);
}

indx_t list_pop_back(List* list, elem_t* elem)
{
    return list_extract_(list, TAIL, elem);
}

indx_t list_pop_front(List* list, elem_t* elem)
{
    return list_extract_(list, HEAD, elem);
}
