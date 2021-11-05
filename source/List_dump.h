#ifndef LIST_DUMP_H
#define LIST_DUMP_H

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>

#include "config.h"
#include "List.h"

void list_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const elem_t*));

void list_dump(List* list, const char msg[]);

#endif // LIST_DUMP_H