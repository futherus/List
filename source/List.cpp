#include "List.h"
#include "List_dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define DATA (list->data_arr)
#define NODS (list->node_arr)
#define TAIL (NODS[-1].next)
#define HEAD (NODS[-1].prev)
#define SIZE (list->size)
#define CAP  (list->capacity)
#define FREE (list->free)
#define SRTD (list->sorted)

static indx_t MIN_LIST_CAP = 8;
static indx_t LIST_CAP_MLTPLR = 2;

#define ASSERT_POS(CONDITION, ERROR, POS)       \
    if(!(CONDITION))                            \
    {                                           \
        list_dump(list, #ERROR, (POS));         \
        return (indx_t) (ERROR);                \
    }                                           \

#define ASSERT(CONDITION, ERROR)                \
    if(!(CONDITION))                            \
    {                                           \
        list_dump(list, #ERROR, ERROR_INDX);    \
        return (indx_t) (ERROR);                \
    }                                           \

static indx_t list_verify_(List* list)
{
    indx_t pos = TAIL;
    for(indx_t iter = 0; iter < 2 * CAP; iter++)
    {
        ASSERT_POS(pos >= -1 && pos < CAP, LIST_BAD_INDX, pos);
        
        ASSERT_POS(NODS[NODS[pos].next].prev == pos, LIST_RIP, pos);

        if(pos == -1)
        {
            ASSERT_POS(iter == SIZE, LIST_MISS, pos);
            
            break;
        }

        ASSERT_POS(iter <= SIZE, LIST_CYCLE, pos);

        pos = NODS[pos].next;
    }

    pos = FREE;
    for(indx_t iter = 0; iter < 2 * CAP; iter++)
    {
        if(pos == -1)
        {
            ASSERT_POS(iter == CAP - SIZE, LIST_FREE_MISS, pos);

            break;
        }

        ASSERT_POS(iter < CAP, LIST_FREE_CYCLE, pos);

        ASSERT_POS(pos >= -1 && pos < CAP, LIST_FREE_BADINDX, pos);

        ASSERT_POS(NODS[pos].prev == INVLD_INDX, LIST_FREE_BADINDX, pos);

        pos = NODS[pos].next;
    }

    return LIST_NOERR;
}

static indx_t list_resize_(List* list, indx_t new_cap)
{
    assert(list && new_cap > 0);

    if(new_cap < MIN_LIST_CAP)
        new_cap = MIN_LIST_CAP;
    if(new_cap == CAP)
        return LIST_NOERR;

    NODS--;
    void* temp_ptr = realloc(NODS, (new_cap + 1) * sizeof(Node));
    if(!temp_ptr)
        return LIST_BAD_ALLOC;

    NODS = ((Node*) temp_ptr) + 1; // shift for list header structure
    
    FREE = CAP;
    for(indx_t iter = FREE; iter < new_cap; iter++)
    {
        NODS[iter].next = iter + 1;
        NODS[iter].prev = INVLD_INDX;
    }
    NODS[new_cap - 1].next = -1;

    CAP = new_cap;

    return LIST_NOERR;
}

static inline indx_t list_insert_(List* list, indx_t pos, elem_t elem)
{
    ASSERT_POS(list, LIST_NULLPTR, pos);
    ASSERT_POS(pos >= -1 && pos < CAP, LIST_BAD_INDX, pos);
    ASSERT_POS(NODS[pos].prev != INVLD_INDX, LIST_ACCESS_FREE, pos);
    indx_t err = list_verify_(list);
    if(err)
        return err;
    
    if(FREE == -1)
        ASSERT_POS(list_resize_(list, CAP * LIST_CAP_MLTPLR) == 0, LIST_BAD_ALLOC, pos);            

    if(pos != HEAD)
        SRTD = false;

    indx_t free = FREE;
    FREE = NODS[FREE].next;

    DATA[free] = elem;

    NODS[free].prev = pos;
    NODS[free].next = NODS[pos].next;

    NODS[NODS[pos].next].prev = free;
    NODS[pos].next = free;

    SIZE++;

    return free;
}

static inline indx_t list_extract_(List* list, indx_t pos, elem_t* elem)
{
    ASSERT_POS(list && elem, LIST_NULLPTR, pos);
    ASSERT_POS(pos >= 0 && pos < CAP, LIST_BAD_INDX, pos);
    ASSERT_POS(NODS[pos].prev != INVLD_INDX, LIST_ACCESS_FREE, pos);
    indx_t err = list_verify_(list);
    if(err)
        return err;

    if(pos != HEAD && pos != TAIL)
        SRTD = false;

    *elem = DATA[pos];

    NODS[NODS[pos].next].prev = NODS[pos].prev;
    NODS[NODS[pos].prev].next = NODS[pos].next;

    NODS[pos].next = FREE;
    NODS[pos].prev = INVLD_INDX;
    FREE = pos;

    SIZE--;
    SRTD = false;

    return pos;
}

static indx_t list_init_cap_(indx_t init_cap) 
{
    if(init_cap < MIN_LIST_CAP)
        return MIN_LIST_CAP;
    
    indx_t cap = MIN_LIST_CAP;
    while(cap < init_cap)
        cap *= LIST_CAP_MLTPLR;

    return cap;
}

indx_t list_init(List* list, elem_t* data, indx_t cap)
{
    ASSERT(list && data, LIST_NULLPTR);

    cap = list_init_cap_(cap);

    NODS++; // shift for list header structure
    ASSERT(list_resize_(list, cap) == 0, LIST_BAD_ALLOC);

    DATA = data;

    TAIL = -1;
    HEAD = -1;
    SIZE =  0;
    SRTD =  1;

    return LIST_NOERR;
}

indx_t list_dstr(List* list)
{
    ASSERT(list, LIST_NULLPTR);

    free(NODS - 1); // shift for list header structure

    return (indx_t) LIST_NOERR;
}

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

static void list_node_swap_(List* list, indx_t indx_1, indx_t indx_2)
{
    Node temp_1 = NODS[indx_1];
    Node temp_2 = NODS[indx_2];

    if(temp_1.prev != INVLD_INDX)
    {
        NODS[temp_1.prev].next = indx_2;
        NODS[temp_1.next].prev = indx_2;
    }

    if(temp_2.prev != INVLD_INDX)
    {
        NODS[temp_2.prev].next = indx_1;
        NODS[temp_2.next].prev = indx_1;
    }

    swap(&NODS[indx_1], &NODS[indx_2], sizeof(Node));
    swap(&DATA[indx_1], &DATA[indx_2], sizeof(elem_t));
}

indx_t list_defragmentation(List* list)
{
    indx_t err = list_verify_(list);
    if(err)
        return err;

    indx_t pos = HEAD;
    for(indx_t iter = 0; iter < SIZE; iter++)
    {
        assert(pos >= 0);

        if(pos != iter)
            list_node_swap_(list, iter, pos);

        pos = NODS[iter].prev;
    }

    FREE = SIZE;
    for(indx_t iter = FREE; iter < CAP; iter++)
        NODS[iter].next = iter + 1;

    NODS[CAP - 1].next = -1;

    err = list_verify_(list);

    SRTD = true;
    return err;
}

indx_t list_find(List* list, indx_t lpos)
{
    ASSERT(lpos >= 0 && lpos < SIZE, LIST_BAD_INDX);

    if(SRTD)
        return TAIL + lpos;
    
    indx_t pos = TAIL;
    for(indx_t iter = 0; iter < lpos; iter++)
        pos = NODS[pos].next;

    return pos;
}

indx_t list_insert(List* list, indx_t pos, elem_t elem)
{
    return list_insert_(list, pos, elem);
}

indx_t list_extract(List* list, indx_t pos, elem_t* elem)
{
    return list_extract_(list, pos, elem);
}

indx_t list_push_back(List* list, elem_t elem)
{
    return list_insert_(list, -1, elem);
}

indx_t list_push_front(List* list, elem_t elem)
{
    return list_insert_(list, HEAD, elem);
}

indx_t list_pop_back(List* list, elem_t* elem)
{
    return list_extract_(list, TAIL, elem);
}

indx_t list_pop_front(List* list, elem_t* elem)
{
    return list_extract_(list, HEAD, elem);
}
