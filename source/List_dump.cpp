#include "List_dump.h"

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>

static FILE* DUMP_STREAM = nullptr;
static void (*PRINT_ELEM)(FILE* dumpstream, const elem_t* elem) = nullptr;

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__)

void list_dump(List* list, const char msg[])
{
    FILE* stream = DUMP_STREAM;
    if(stream == nullptr)
        return;

    PRINT("%s\n", msg);
    PRINT("tail: %4ld | head: %4ld\n", list->node_arr[0].next, list->node_arr[0].prev);

    PRINT("indx: ");
    for(indx_t iter = 0; iter < list->capacity; iter++)
        PRINT("%5ld|", iter);

    PRINT("\n");

    PRINT("data: ");
    for(indx_t iter = 0; iter < list->capacity; iter++)
        PRINT("%5lg|", list->data_arr[iter]);

    PRINT("\n");

    PRINT("next: ");
    for(indx_t iter = 0; iter < list->capacity; iter++)
        PRINT("%5ld|", list->node_arr[iter].next);

    PRINT("\n");

    PRINT("prev: ");
    for(indx_t iter = 0; iter < list->capacity; iter++)
        PRINT("%5ld|", list->node_arr[iter].prev);

    PRINT("\n");
    
    indx_t cur_indx = list->node_arr[0].next; //tail
    PRINT(">>>>: ");
    for(indx_t iter = 0; iter < list->capacity; iter++)
    {
        if(cur_indx == 0)
            break;
            
        PRINT("%5lg|", list->data_arr[cur_indx]);
        cur_indx = list->node_arr[cur_indx].next;
    }

    PRINT("\n");

    cur_indx = list->node_arr[0].prev; // head
    PRINT("<<<<: ");
    for(indx_t iter = 0; iter < list->capacity; iter++)
    {
        if(cur_indx == 0)
            break;

        PRINT("%5lg|", list->data_arr[cur_indx]);
        cur_indx = list->node_arr[cur_indx].prev;
    }

    PRINT("\n\n");
}

static void close_dumpfile_()
{
    if(fclose(DUMP_STREAM) != 0)
        perror("List dump file can't be succesfully closed");
}

void list_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const elem_t*))
{
    if(print_func)
        PRINT_ELEM = print_func;

    if(dumpstream)
    {
        DUMP_STREAM = dumpstream;
        return;
    }

    if(LIST_DUMPFILE[0] != 0)
    {
        DUMP_STREAM = fopen(LIST_DUMPFILE, "w");

        if(DUMP_STREAM)
        {
            atexit(&close_dumpfile_);
            return;
        }
    }

    perror("Can't open dump file");
    DUMP_STREAM = stderr;

    return;
}
