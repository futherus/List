#include <stdio.h>
#include "List.h"
#include "List_dump.h"

int main()
{
    List list = {};
    list_dump_init(nullptr, nullptr);

    double temp = 0;
    double buffer[100] = {0};
    list_init(&list, buffer, 8);

    //for(indx_t iter = 0; iter < 20; iter++)
    //{
    //    //list_push_back(&list, -iter);
    //    list_push_front(&list, iter);
    //}
    //list_dump(&list, "after push\n");
//
    //for(indx_t iter = 0; iter < 10; iter++)
    //{
    //    //list_extract(&list, iter, &temp);
    //}
    //list_dump(&list, "after extract\n");
    list_push_back(&list, 10);
    list_push_back(&list, 20);
    list_push_back(&list, 30);
    list_push_back(&list, 40);
    list_push_back(&list, 50);

    list_dump(&list, "before defragmentation");
    list_defragmentation(&list);

    list_dump(&list, "defrgmtation\n");


    return 0;
}
