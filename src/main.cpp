#include <stdio.h>
#include "List.h"
#include "List_dump.h"

int main()
{
    List list = {};
    double temp = 0;
    list_dump_init(nullptr, nullptr);
    list_init(&list, 0);

     list_push_back(&list, 77);
     list_dump(&list, "after push\n");

    for(indx_t iter = 0; iter < 10; iter++)
    {
        list_insert(&list, 0, iter);
    }

    list_dump(&list, "before defragmentation\n");

    list_defragmentation(&list);

    list_dump(&list, "defrgmtation\n");

    list_extract(&list, 1, &temp);

     list_dstr(&list);
    return 0;
}
