#ifndef LIST_CONFIG_H
#define LIST_CONFIG_H

#include "../../../src/Hashtable_config.h"

//-----------------------------------------------------------------------------
#define GRAPHVIZ_ENABLE
//#define LIST_VERIFICATION

#ifdef GRAPHVIZ_ENABLE
const int  GRAPHVIZ_NAME_SIZE      = 300;
const char GRAPHVIZ_PNG_NAME[]     = "graphviz_dump";
const char GRAPHVIZ_TMP_NAME[]     = "graphviz_temp";
#endif // GRAPHVIZ_ENABLE

const char LIST_DEFAULT_DUMPFILE[] = "list_dump.html";
const int  MIN_LIST_CAP    = 2;
const int  LIST_CAP_MLTPLR = 2;

//-----------------------------------------------------------------------------
const size_t KEY_SIZE = 32;

typedef struct
{
    char key[KEY_SIZE] = {};
    ht_elem_t obj = {};
} list_elem_t;

#endif // LIST_CONFIG_H
