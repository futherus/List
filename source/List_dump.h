#ifndef LIST_DUMP_H
#define LIST_DUMP_H

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>

#include "config.h"
#include "List.h"

const indx_t DEFLT_INDX = -777;
const indx_t ERROR_INDX = -778;

void list_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const elem_t*));

void list_dump(List* list, const char msg[], indx_t err_pos = DEFLT_INDX);

#endif // LIST_DUMP_H
