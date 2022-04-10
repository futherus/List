#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "List.h"

//-----------------------------------------------------------------------------
#ifdef GRAPHVIZ_ENABLE
static const char GRAPHVIZ_INTRO[] =
R"(
digraph G{
    graph [dpi = 100];
    bgcolor = "#2F353B";
    ranksep = 1.5;
    splines = ortho;
    edge[minlen = 3, arrowsize = 2, penwidth = 1.5];
    node[shape = rectangle, style = "rounded, filled", height = 3,
    width = 2.3, fixedsize = true, fillcolor = "#C5D0E6", fontsize = 30,
    color = "#C5D0E6", penwidth = 3];
    {
        rank = min; node0[style = invis, height = 0.2];
    }
    {
        rank = max; node1[style = invis, height = 0.2];
    }
)";

static const char GRAPHVIZ_OUTRO[] = "}\n";
#endif // GRAPHVIZ_ENABLE
//-----------------------------------------------------------------------------

static const char HTML_INTRO[] =
R"(
<html>
    <head>
        <title>
            List log
        </title>
        <style>
            .ok {color: springgreen;font-weight: bold;}
            .error{color: red;font-weight: bold;}
            .log{color: #C5D0E6;}
            .title{color: #E59E1F;text-align: center;font-weight: bold;}
        </style>
    </head>
    <body bgcolor = "#2F353B">
        <pre class = "log">
)";

static const char HTML_OUTRO[] =
R"(
        </pre>
    </body>
</html>
)";

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__)

#define NODS (list->node_arr)
#define TAIL (NODS[-1].prev)
#define HEAD (NODS[-1].next)
#define FREE (list->free)
#define SIZE (list->size)
#define CAP  (list->capacity)

static FILE* DUMP_STREAM  = nullptr;
static void (*PRINT_ELEM)(FILE* dumpstream, const elem_t* elem) = nullptr;

//-----------------------------------------------------------------------------
#ifdef GRAPHVIZ_ENABLE
static char* graphviz_png_(int dump_iter)
{
    static char filename[GRAPHVIZ_NAME_SIZE] = "";
    
    sprintf(filename, "%s_%d.png", GRAPHVIZ_PNG_NAME, dump_iter);

    return filename;
}

static void list_graph_dump_(List* list, int dump_iter)
{
    FILE* stream = fopen(GRAPHVIZ_TMP_NAME, "w");
    
    PRINT("%s", GRAPHVIZ_INTRO);

    PRINT("node0 -> label%p[style = invis];\n"
          "node1 -> label%p[style = invis];", 
          &NODS[-1], &NODS[-1]);

    PRINT("{rank = same;\n"
          "label%p[label = \"header \n\n head: %d \n tail: %d \n free: %d\", color = \"red\"];\n",
           &NODS[-1], NODS[-1].next, NODS[-1].prev, FREE);

    for(int iter = 0; iter < CAP; iter++)
    {
        PRINT("label%p[label = \"idx: %d \n\n", &NODS[iter], iter);
        if(PRINT_ELEM)
            PRINT_ELEM(stream, &NODS[iter].data);
        else
            PRINT("--//--");
        PRINT("\n nxt: %d \n prv: %d\"];\n", NODS[iter].next, NODS[iter].prev);

        PRINT("label%p -> label%p[weight = 5, style = invis];\n", &NODS[iter - 1], &NODS[iter]);
    }

    PRINT("}\n");

    for(int iter = 0; iter < CAP; iter++)
    {
        if(NODS[iter].prev != LIST_INVLD_INDX)        
        {
            PRINT("label%p -> label%p [color = \"green\"];\n", &NODS[iter], &NODS[NODS[iter].next]);
            PRINT("label%p -> label%p [color = \"orange\", dir = \"back\"];\n", &NODS[NODS[iter].prev], &NODS[iter]);
        }
        else
            PRINT("label%p -> label%p [color = \"white\"];\n", &NODS[iter], &NODS[NODS[iter].next]);
    }

    PRINT("label%p -> label%p [color = \"green\", penwidth = 3.5];\n", &NODS[-1], &NODS[NODS[-1].next]);
    PRINT("label%p -> label%p [color = \"orange\", dir = \"back\", penwidth = 3.5];\n", &NODS[NODS[-1].prev], &NODS[-1]);

    PRINT("label%p -> label%p [color = \"white\", penwidth = 3.5];\n", &NODS[-1], &NODS[FREE]);

    PRINT(GRAPHVIZ_OUTRO);

    fclose(stream);

    char sys_cmd[GRAPHVIZ_NAME_SIZE] = {}; 
    sprintf(sys_cmd, "dot %s -q -Tpng -o %s", GRAPHVIZ_TMP_NAME, graphviz_png_(dump_iter));

    system(sys_cmd);
}
#endif // GRAPHVIZ_ENABLE
//-----------------------------------------------------------------------------


static const char DATA_IS_NULL_MSG[] = "\n                                                                                            "
                                       "\n  DDDDDD      A    TTTTTTTTT    A         IIIII   SSSSS     N    N  U     U  L      L       "
                                       "\n  D     D    A A       T       A A          I    S          NN   N  U     U  L      L       "
                                       "\n  D     D   A   A      T      A   A         I     SSSS      N N  N  U     U  L      L       "
                                       "\n  D     D  AAAAAAA     T     AAAAAAA        I         S     N  N N  U     U  L      L       "
                                       "\n  DDDDDD  A       A    T    A       A     IIIII  SSSSS      N   NN   UUUUU   LLLLLL LLLLLL  "
                                       "\n                                                                                            ";

void list_dump(List* list, const char* msg, int err_pos)
{
#ifdef GRAPHVIZ_ENABLE
    static int dump_iter = 0;
#endif // GRAPHVIZ_ENABLE

    assert(list);

    FILE* stream = DUMP_STREAM;
    if(stream == nullptr)
        return;

    PRINT("<span class = \"title\">----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------</span>\n");
    
    if(msg)
    {
        if(err_pos == LIST_DEFLT)
            PRINT("<span class = \"title\"> %s </span>\n", msg);
        else if(err_pos == LIST_ERROR)
            PRINT("<span class = \"error\"> %s </span>\n", msg);
        else
            PRINT("<span class = \"error\"> %s (%d) </span>\n", msg, err_pos);
    }
    
    if(!NODS)
    {
        PRINT("<span class = \"error\">%s\n</span>", DATA_IS_NULL_MSG);
        PRINT("<span class = \"title\">\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n</span>");
        return;
    }

    PRINT("tail: %5d\n" "head: %5d\n" "free: %5d\n" "size: %5d\n" "cap:  %5d\n\n",
           TAIL, HEAD, FREE, SIZE, CAP);

    PRINT("indx: ");
    for(int iter = 0; iter < CAP; iter++)
        PRINT("%5d|", iter);

    PRINT("\n");

    PRINT("next: ");
    for(int iter = 0; iter < CAP; iter++)
        PRINT("%5d|", NODS[iter].next);

    PRINT("\n");

    PRINT("prev: ");
    for(int iter = 0; iter < CAP; iter++)
        PRINT("%5d|", NODS[iter].prev);

    PRINT("\n\n");
    
    if(PRINT_ELEM)
    {
        int cur_indx = HEAD;
        PRINT(">>>>: ");
        for(int iter = 0; iter < CAP; iter++)
        {
            if(cur_indx == LIST_HEADER_POS)
                break;

            PRINT("`"); 
            PRINT_ELEM(stream, &NODS[cur_indx].data);
            PRINT("` ");
            cur_indx = NODS[cur_indx].next;
        }

        PRINT("\n");

        cur_indx = TAIL;
        PRINT("<<<<: ");
        for(int iter = 0; iter < CAP; iter++)
        {
            if(cur_indx == LIST_HEADER_POS)
                break;

            PRINT("`"); 
            PRINT_ELEM(stream, &NODS[cur_indx].data);
            PRINT("` ");
            cur_indx = NODS[cur_indx].prev;
        }
        PRINT("\n\n");
    }

#ifdef GRAPHVIZ_ENABLE
    list_graph_dump_(list, dump_iter);
    PRINT(R"(<img src = ")" "%s" R"(" alt = "Graphical dump" height = 400>)", graphviz_png_(dump_iter));
    dump_iter++;
#endif // GRAPHVIZ_ENABLE

    PRINT("<span class = \"title\">\n----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n</span>");

    return;
}

#undef PRINT

static void close_dumpfile_()
{
    fprintf(DUMP_STREAM, "%s", HTML_OUTRO);

    if(fclose(DUMP_STREAM) != 0)
        perror("List dump: file was closed unsuccessfully");
}

FILE* list_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const elem_t*))
{
    if(print_func)
        PRINT_ELEM = print_func;

    if(dumpstream)
    {
        DUMP_STREAM = dumpstream;
        return DUMP_STREAM;
    }

    if(LIST_DEFAULT_DUMPFILE[0] != 0)
    {
        DUMP_STREAM = fopen(LIST_DEFAULT_DUMPFILE, "w");

        if(DUMP_STREAM)
        {
            atexit(&close_dumpfile_);

            fprintf(DUMP_STREAM, "%s", HTML_INTRO);
            return DUMP_STREAM;
        }
    }

    perror("List dump: cannot open dump file");

    return nullptr;
}
