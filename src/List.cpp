#include "List.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define DATA (list->data_arr)
#define NODS (list->node_arr)
#define TAIL (NODS[LIST_HEADER_POS].prev)
#define HEAD (NODS[LIST_HEADER_POS].next)
#define SIZE (list->size)
#define CAP  (list->capacity)
#define FREE (list->free)
#define SRTD (list->sorted)

#define ASSERT(CONDITION, ERROR)                                    \
    if(!(CONDITION))                                                \
    {                                                               \
        fprintf(stderr, "\x1b[31;1mERROR:\x1b[0m %s\n", #ERROR);    \
        return (ERROR);                                             \
    }                                                               \

#ifdef LIST_VERIFICATION
#define VERIFY(LIST)                            \
    {                                           \
        int err = list_verify(LIST);            \
        if(err)                                 \
            return err;                         \
    }                                           
#else
#define VERIFY(LIST) {void();}
#endif // LIST_VERIFICATION

static int list_resize_(List* list, int new_cap)
{
    assert(list && new_cap >= CAP);

    if(new_cap < MIN_LIST_CAP)
        new_cap = MIN_LIST_CAP;
    if(new_cap == CAP)
        return LIST_NOERR;

    NODS--;                        // shift for list header structure
    Node* tmp_ptr = (Node*) aligned_alloc(LIST_ALIGNMENT, (size_t) (new_cap + 1) * sizeof(Node));
    if(!tmp_ptr)
        return LIST_BAD_ALLOC;
    
    if(NODS != nullptr)
    {
        memcpy(tmp_ptr, NODS, (CAP + 1) * sizeof(Node));
        free(NODS);
    }

    NODS = tmp_ptr + 1; // shift for list header structure

    FREE = CAP;
    for(int iter = FREE; iter < new_cap; iter++)
    {
        NODS[iter].next = iter + 1;
        NODS[iter].prev = LIST_INVLD_INDX;
        // NODS[iter].data = {};
    }
    NODS[new_cap - 1].next = LIST_HEADER_POS;

    CAP = new_cap;

    return LIST_NOERR;
}

int list_ctor(List* list, int init_cap)
{
    assert(list);
    ASSERT(!NODS, LIST_REINIT);

    if(init_cap < MIN_LIST_CAP)
        init_cap = MIN_LIST_CAP;

    // list_ctor() used list_resize_(), however it had to do NODS-- to create suitable
    // pointer for list_resize_(). NODS-- appeared to be UB, because it used ariphmetic on 
    // nullptr. Therefore initializing aligned_alloc() was made distinguished.

    Node* tmp_ptr = (Node*) aligned_alloc(LIST_ALIGNMENT, (size_t) (init_cap + 1) * sizeof(Node));
    if(!tmp_ptr)
        return LIST_BAD_ALLOC;

    NODS = tmp_ptr + 1; // shift for list header structure

    FREE = CAP;
    for(int iter = FREE; iter < init_cap; iter++)
    {
        NODS[iter].next = iter + 1;
        NODS[iter].prev = LIST_INVLD_INDX;
        // NODS[iter].data = {};
    }
    NODS[init_cap - 1].next = LIST_HEADER_POS;

    CAP = init_cap;
    TAIL = -1;
    HEAD = -1;
    SIZE =  0;
    SRTD =  1;

    return LIST_NOERR;
}

void list_dtor(List* list)
{
    if(NODS != nullptr)
        free(NODS - 1); // shift for list header structure

    CAP  = LIST_INVLD_INDX;
    SIZE = LIST_INVLD_INDX;
    FREE = LIST_INVLD_INDX;
    NODS = nullptr;

    return;
}

#define ASSERT_POS(CONDITION, ERROR, POS)       \
    if(!(CONDITION))                            \
    {                                           \
        list_dump(list, #ERROR, (POS));         \
        return (ERROR);                         \
    }                                           \

int list_verify(List* list)
{
    assert(list);
    ASSERT_POS(NODS || CAP == 0, LIST_NOT_INIT, -1);

    int pos = TAIL;
    for(int iter = 0; iter < 2 * CAP; iter++)
    {
        ASSERT_POS(pos >= LIST_HEADER_POS && pos < CAP, LIST_BAD_INDX, pos);
        
        ASSERT_POS(NODS[NODS[pos].prev].next == pos, LIST_RIP, pos);

        if(pos == LIST_HEADER_POS)
        {
            ASSERT_POS(iter == SIZE, LIST_MISS, pos);
            
            break;
        }

        ASSERT_POS(iter <= SIZE, LIST_CYCLE, pos);

        pos = NODS[pos].prev;
    }

    pos = FREE;
    for(int iter = 0; iter < 2 * CAP; iter++)
    {
        if(pos == LIST_HEADER_POS)
        {
            ASSERT_POS(iter == CAP - SIZE, LIST_FREE_MISS, pos);

            break;
        }

        ASSERT_POS(iter < CAP, LIST_FREE_CYCLE, pos);

        ASSERT_POS(pos >= LIST_HEADER_POS && pos < CAP, LIST_FREE_BADINDX, pos);

        ASSERT_POS(NODS[pos].prev == LIST_INVLD_INDX, LIST_FREE_BADINDX, pos);

        pos = NODS[pos].next;
    }

    return LIST_NOERR;
}

#undef ASSERT_POS

/** \brief  Swaps any type

    \param [in] [out] first  Pointer to first object
    \param [in] [out] second Pointer to second object
    \param [in]       size   Size of passed objects in bytes
*/
static void swap(void* first, void* second, size_t size)
{
    assert(first && second);

    #define SWAP_ITERATION(type)                                     \
    {                                                                \
        type temp = *((type*) first);                                \
        *((type*) first) = *((type*) second);                        \
        *((type*) second) = temp;                                    \
                                                                     \
        first  = (void*) ((unsigned char*) (first)  + sizeof(type)); \
        second = (void*) ((unsigned char*) (second) + sizeof(type)); \
        size  -= sizeof(type);                                       \
    }                                                                \

    while(size >= 0x8)
        SWAP_ITERATION(int64_t);

    while(size >= 0x4)
        SWAP_ITERATION(int32_t);

    while(size >= 0x2)
        SWAP_ITERATION(int16_t);

    while(size >= 0x1)
        SWAP_ITERATION(int8_t);

    #undef SWAP_ITERATION
}

static void list_node_swap_(List* list, int indx_1, int indx_2)
{
    Node temp_1 = NODS[indx_1];
    Node temp_2 = NODS[indx_2];

    if(temp_1.prev != LIST_INVLD_INDX)
    {
        NODS[temp_1.next].prev = indx_2;
        NODS[temp_1.prev].next = indx_2;
    }

    if(temp_2.prev != LIST_INVLD_INDX)
    {
        NODS[temp_2.next].prev = indx_1;
        NODS[temp_2.prev].next = indx_1;
    }

    swap(&NODS[indx_1], &NODS[indx_2], sizeof(Node));
}

int list_defragmentation(List* list)
{
    VERIFY(list);

    int pos = HEAD;
    for(int iter = 0; iter < SIZE; iter++)
    {
        assert(pos >= 0);

        if(pos != iter)
            list_node_swap_(list, iter, pos);

        pos = NODS[iter].next;
    }

    FREE = SIZE;
    for(int iter = FREE; iter < CAP; iter++)
        NODS[iter].next = iter + 1;

    NODS[CAP - 1].next = LIST_HEADER_POS;

    VERIFY(list);

    SRTD = true;

    return LIST_NOERR;
}

int list_find(List* list, int lpos, list_elem_t* elem)
{
    VERIFY(list);
    ASSERT(lpos >= 0 && lpos < SIZE, LIST_BAD_INDX);

    if(SRTD)
        return HEAD + lpos;
    
    int pos = HEAD;
    for(int iter = 0; iter < lpos; iter++)
        pos = NODS[pos].next;

    *elem = NODS[pos].data;
    return pos;
}

int list_insert(List* list, int pos, list_elem_t elem)
{
    VERIFY(list);
    ASSERT(pos >= LIST_HEADER_POS && pos < CAP, LIST_BAD_INDX);
    ASSERT(NODS[pos].next != LIST_INVLD_INDX, LIST_ACCESS_FREE);

    if(FREE == LIST_HEADER_POS)
        ASSERT(list_resize_(list, CAP * LIST_CAP_MLTPLR) == 0, LIST_BAD_ALLOC);            

    if(pos != TAIL)
        SRTD = false;

    int free = FREE;
    FREE = NODS[FREE].next;

    NODS[free].data = elem;

    NODS[free].prev = pos;
    NODS[free].next = NODS[pos].next;

    NODS[NODS[pos].next].prev = free;
    NODS[pos].next = free;

    SIZE++;

    return free;
}

int list_push_back(List* list, list_elem_t elem)
{
    return list_insert(list, TAIL, elem);
}

int list_push_front(List* list, list_elem_t elem)
{
    return list_insert(list, LIST_HEADER_POS, elem);
}

int list_delete(List* list, int pos)
{
    VERIFY(list);
    ASSERT(pos >= 0 && pos < CAP, LIST_BAD_INDX);
    ASSERT(NODS[pos].next != LIST_INVLD_INDX, LIST_ACCESS_FREE);
    ASSERT(SIZE > 0, LIST_EMPTY);

    if(pos != HEAD && pos != TAIL)
        SRTD = false;

    // NODS[pos].data = {};

    NODS[NODS[pos].prev].next = NODS[pos].next;
    NODS[NODS[pos].next].prev = NODS[pos].prev;

    NODS[pos].next = FREE;
    NODS[pos].prev = LIST_INVLD_INDX;
    FREE = pos;

    SIZE--;

    return pos;
}

int list_pop_back(List* list)
{
    return list_delete(list, TAIL);
}

int list_pop_front(List* list)
{
    return list_delete(list, HEAD);
}

int list_get(List* list, int pos, list_elem_t* elem)
{
    VERIFY(list);
    ASSERT(pos >= LIST_HEADER_POS && pos < CAP, LIST_BAD_INDX);
    ASSERT(NODS[pos].next != LIST_INVLD_INDX, LIST_ACCESS_FREE);

    if(elem && pos != LIST_HEADER_POS)
        *elem = NODS[pos].data;
    
    return pos;
}

int list_next(List* list, int pos, list_elem_t* elem)
{
    return list_get(list, NODS[pos].next, elem);
}

int list_prev(List* list, int pos, list_elem_t* elem)
{    
    return list_get(list, NODS[pos].prev, elem);
}

int list_back(List* list, list_elem_t* elem)
{
    return list_get(list, TAIL, elem);
}

int list_front(List* list, list_elem_t* elem)
{
    return list_get(list, HEAD, elem);
}
