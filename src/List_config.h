#ifndef LIST_CONFIG_H
#define LIST_CONFIG_H

#include <immintrin.h>
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

// Nodes alignment
const size_t LIST_ALIGNMENT = 32;

// Structure list_elem_t alligns on 32 bytes because of __m256i member =>
// => (sizeof(list_elem_t) = 64).
// Therefore Node also alligns on 32 bytes => sizeof(Node) = 96.
typedef struct __attribute__((aligned(LIST_ALIGNMENT)))
{
    __m256i key;
    ht_elem_t obj = {};
} list_elem_t;

// __attribute_alloc_align__; 

#endif // LIST_CONFIG_H
