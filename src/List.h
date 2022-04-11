#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdio.h>

#include "List_config.h"

struct Node
{
    int next = 0;
    int prev = 0;

    list_elem_t data = {};
};

struct List 
{
    int capacity = 0;
    int size     = 0;
    int free     = 0;
    bool   sorted   = 0;

    Node* node_arr = nullptr;
};

// position of header structure
const int LIST_HEADER_POS = -1;
// index for filling invalid fields
const int LIST_INVLD_INDX = -666;
// indexes for dump
const int LIST_DEFLT = -777;
const int LIST_ERROR = -778;

enum List_err
{
    LIST_NOERR        =    0,
    LIST_BAD_ALLOC    =  -10,
    LIST_BAD_INDX     =  -20,
    LIST_MISS         =  -30,
    LIST_CYCLE        =  -40,
    LIST_RIP          =  -50,
    LIST_FREE_MISS    =  -60,
    LIST_FREE_CYCLE   =  -70,
    LIST_FREE_BADINDX =  -80,
    LIST_ACCESS_FREE  =  -90,
    LIST_REINIT       = -100,
    LIST_NOT_INIT     = -110,
    LIST_EMPTY        = -120,
};

int  list_ctor(List* list, int init_cap = MIN_LIST_CAP);
void list_dtor(List* list);

/** Initializing list dump
 *  \param dumpstream stream for dump (if dumpstream == nullptr new file will be opened)
 *  \param print_func function for printing list element
**/ 
FILE* list_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const list_elem_t*));
void  list_dump(List* list, const char* msg = nullptr, int err_pos = LIST_DEFLT);
int   list_verify(List* list);

/** Inserting element
 *  \param pos  physical position of element to insert after
 *  \param elem destination pointer
 * 
 *  \return     physical position of inserted element on success
 *              error code otherwise
**/
int  list_insert    (List* list, int pos, list_elem_t  elem);
int  list_push_front(List* list, list_elem_t  elem);
int  list_push_back (List* list, list_elem_t  elem);

/** Deleting element
 *  \param pos  physical position of element to delete
 *  \param elem destination pointer
 * 
 *  \return     0 on success
 *              error code otherwise
**/
int  list_delete   (List* list, int pos);
int  list_pop_front(List* list);
int  list_pop_back (List* list);

/** Getting element
 *  \param  pos  physical position of desired element
 *  \param  elem destination pointer
 * 
 *  \return      physical position of return element on success
 *               LIST_HEADER_POS if end of list was met
 *               error code otherwise
**/
int  list_get       (List* list, int pos, list_elem_t* elem = nullptr);
int  list_next      (List* list, int pos, list_elem_t* elem = nullptr);
int  list_prev      (List* list, int pos, list_elem_t* elem = nullptr);
int  list_back      (List* list, list_elem_t* elem = nullptr);
int  list_front     (List* list, list_elem_t* elem = nullptr);

/** Getting element
 *  \param lpos logical position of desired element
 *  \param elem destination pointer
 * 
 *  \return     physical position of return element on success
 *              error code otherwise
**/
int  list_find      (List* list, int lpos, list_elem_t* elem = nullptr);

/** Sorting elements according to logical positions
 *  \return     0 on success
 *              error code otherwise
**/
int  list_defragmentation(List* list);

#endif // LIST_H
