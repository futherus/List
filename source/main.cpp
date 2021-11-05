#include <stdio.h>
#include "List.h"
#include "List_dump.h"

int main()
{
    List list = {};
    list_dump_init(nullptr, nullptr);
    
    double buffer[100] = {0};
    list_init(&list, buffer, 100);

    list_push_front(&list, 11);
    list_push_front(&list, 12);
    list_push_front(&list, 13);
    list_push_front(&list, 14);

    double temp = 0;
    list_dump(&list, "BEFORE\n");
    list_pop_front(&list, &temp);
    list_dump(&list, "EXTRACT 1\n");
    list_extract(&list, 2, &temp);
    list_dump(&list, "EXTRACT 2\n");
    list_extract(&list, 3, &temp);
    list_dump(&list, "EXTRACT 3\n");
    list_extract(&list, 4, &temp);
    list_dump(&list, "EXTRACT 4\n");    

    return 0;
}