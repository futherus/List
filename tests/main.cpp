#include <stdio.h>
#include <string.h>

#include "../src/List.h"
#include "../utils/logs/logs.h"

void print_elem(FILE* stream, const list_elem_t* elem)
{
    fprintf(stream, "ptr = %p, " "key = %s", elem->obj, elem->key);
}

int main()
{
    List list = {};
    logs_init("logs.html");
    list_dump_init(logs_get(), &print_elem);
    list_ctor(&list, 0);

    list_elem_t val = {};
    memcpy(val.key, "Keyvalue", sizeof("Keyvalue"));
    list_push_back(&list, val);
    list_dump(&list, nullptr);

    for(int iter = 0; iter < 10; iter++)
    {
        val.obj = (void*) iter;
        list_insert(&list, 0, val);
    }

    printf("list_front: %d\n", list_get(&list, 3, &val));
    print_elem(stdout, &val);
    printf("\n");

    list_dump(&list, "before defragmentation\n");

    list_defragmentation(&list);

    list_dump(&list, "defragmentation\n");

    list_dtor(&list);

    return 0;
}
