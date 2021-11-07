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

    list_push_front(&list, 10);
    list_push_front(&list, 11);
    list_push_front(&list, 12);
    list_push_back(&list, 77);
    //list.node_arr[2].next = 11;
    list_push_front(&list, 13);
    list_push_front(&list, 14);
    //list_dump(&list, "BEFORE\n");
    list_push_front(&list, 15);
    list_push_front(&list, 16);
    list_push_front(&list, 17);
    list_push_front(&list, 18);
    
    list_extract(&list, 2, &temp);
    list_dump(&list, "BEFORE_DEFRAGMENTATION\n");

    list_defragmentation(&list);

    list_dump(&list, "AFTER_DEFRAGMENTATION\n");

    list_pop_front(&list, &temp);
    list_dump(&list, "EXTRACT 1\n");
    //list_dump(&list, "EXTRACT 2\n");
    list_extract(&list, 3, &temp);
    //list_dump(&list, "EXTRACT 3\n");
    list_extract(&list, 4, &temp);
    list_dump(&list, "EXTRACT 4\n");  
    list_defragmentation(&list);
    list_dump(&list, "AFTER_DEFRAGMENTATION\n");


    return 0;
}