#include "List_dump.h"

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char GRAPHVIZ_OUTRO[] = "}\n";

static const int GRAPHVIZ_NAME_SIZE = 300;

static long DUMP_ITERATION = 0;
static const char GRAPHVIZ_PNG_NAME[] = "list_dump/graphviz_dump";
static const char LIST_DUMPFILE[] = "list_dump.html";

static char* graphviz_png_()
{
    static char filename[GRAPHVIZ_NAME_SIZE] = "";
    sprintf(filename, "%s_%ld.png", GRAPHVIZ_PNG_NAME, DUMP_ITERATION);

    return filename;
}
static const char GRAPHVIZ_INTRO[] =
R"(
digraph G{
    graph [dpi = 70];
    bgcolor = "#2F353B";
    ranksep = 1.5;
    splines = ortho;
    edge[minlen = 3, labeldistance = 3, arrowsize = 2, penwidth = 1.5];
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

static FILE* DUMP_STREAM    = nullptr;
static void (*PRINT_ELEM)(FILE* dumpstream, const elem_t* elem) = nullptr;

#define PRINT(format, ...) fprintf(stream, format, ##__VA_ARGS__)

#define DATA (list->data_arr)
#define NODS (list->node_arr)
#define FRES (list->free_arr)
#define TAIL (NODS[-1].next)
#define HEAD (NODS[-1].prev)
#define FREE (list->free)
#define SIZE (list->size)
#define CAP  (list->capacity)

static void list_graph_dump_(List* list)
{
    FILE* stream = fopen("list_dump/graphviz_temp.txt", "w");
    
    PRINT("%s", GRAPHVIZ_INTRO);

    PRINT("node0 -> label%p[style = invis];\n"
          "node1 -> label%p[style = invis];", 
          &NODS[-1], &NODS[-1]);

    PRINT("{rank = same;\n"
          "label%p[label = \"header \n\n tail: %ld \n head: %ld \n free: %ld\", color = \"red\"];\n",
           &NODS[-1], NODS[-1].next, NODS[-1].prev, FREE);

    for(indx_t iter = 0; iter < CAP; iter++)
    {
        PRINT("label%p[label = \"idx: %ld \n\n %lg \n nxt: %ld \n prv: %ld\"];\n",
               &NODS[iter], iter, DATA[iter], NODS[iter].next, NODS[iter].prev);

        PRINT("label%p -> label%p[weight = 5, style = invis];\n", &NODS[iter - 1], &NODS[iter]);
    }

    PRINT("}\n");

    for(indx_t iter = 0; iter < CAP; iter++)
    {
        if(NODS[iter].prev != INVLD_INDX)        
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

    char sys_cmd[GRAPHVIZ_NAME_SIZE] = "dot list_dump/graphviz_temp.txt -q -Tpng -o ";
    strcat(sys_cmd, graphviz_png_());

    system(sys_cmd);
}

void list_dump(List* list, const char msg[], indx_t err_pos)
{
    DUMP_ITERATION++;

    FILE* stream = DUMP_STREAM;
    if(stream == nullptr)
        return;

    PRINT("<span class = \"title\">--------------------------------------------------------------------------------------------------------------------------------</span>\n");
    
    if(err_pos == DEFLT_INDX)
        PRINT("<span class = \"title\"> %s </span>\n", msg);
    else if(err_pos == ERROR_INDX)
        PRINT("<span class = \"error\"> %s </span>\n", msg);
    else
        PRINT("<span class = \"error\"> %s (%ld) </span>\n", msg, err_pos);

    PRINT("tail: %5ld\n" "head: %5ld\n" "size: %5ld\n" "cap:  %5ld\n" "free: %5ld\n\n", TAIL, HEAD, SIZE, CAP, FREE);

    PRINT("indx: ");
    for(indx_t iter = 0; iter < CAP; iter++)
        PRINT("%5ld|", iter);

    PRINT("\n");

    PRINT("data: ");
    for(indx_t iter = 0; iter < CAP; iter++)
        PRINT("%5lg|", list->data_arr[iter]);

    PRINT("\n");

    PRINT("next: ");
    for(indx_t iter = 0; iter < CAP; iter++)
        PRINT("%5ld|", list->node_arr[iter].next);

    PRINT("\n");

    PRINT("prev: ");
    for(indx_t iter = 0; iter < CAP; iter++)
        PRINT("%5ld|", list->node_arr[iter].prev);

    PRINT("\n\n");
    
    indx_t cur_indx = TAIL;
    PRINT(">>>>: ");
    for(indx_t iter = 0; iter < CAP; iter++)
    {
        if(cur_indx == -1)
            break;
            
        PRINT("%5lg|", DATA[cur_indx]);
        cur_indx = NODS[cur_indx].next;
    }

    PRINT("\n");

    cur_indx = HEAD;
    PRINT("<<<<: ");
    for(indx_t iter = 0; iter < CAP; iter++)
    {
        if(cur_indx == -1)
            break;

        PRINT("%5lg|", DATA[cur_indx]);
        cur_indx = NODS[cur_indx].prev;
    }
    PRINT("\n\n");

    list_graph_dump_(list);

    PRINT(R"(<img src = ")" "%s" R"(" alt = "Graphical dump" height = 400>)", graphviz_png_());

    PRINT("<span class = \"title\">\n--------------------------------------------------------------------------------------------------------------------------------\n</span>");
}

static void close_dumpfile_()
{
    fprintf(DUMP_STREAM, "%s", HTML_OUTRO);

    if(fclose(DUMP_STREAM) != 0)
        perror("List dump file can't be succesfully closed");
}

void list_dump_init(FILE* dumpstream, void(*print_func)(FILE*, const elem_t*))
{
    if(print_func)
        PRINT_ELEM = print_func;

    if(dumpstream)
    {
        DUMP_STREAM = dumpstream;
        return;
    }

    if(LIST_DUMPFILE[0] != 0)
    {
        DUMP_STREAM = fopen(LIST_DUMPFILE, "w");

        fprintf(DUMP_STREAM, "%s", HTML_INTRO);

        if(DUMP_STREAM)
        {
            atexit(&close_dumpfile_);
            return;
        }
    }

    perror("Can't open dump file");
    DUMP_STREAM = stderr;

    return;
}
