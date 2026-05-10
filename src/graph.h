#ifndef GRAPH_H
#define GRAPH_H

#include "raylib.h"

typedef struct edge {
    int dest;
    int weight;
    struct edge* next;
} edge;

typedef struct graph {
    int node_num;
    edge** adjacency_list;
} graph;

typedef struct {
    graph* g;
    int source;
    int destination;
} graph_load_data;

graph* create_graph(int node_num);
void add_edge(graph* g, int src, int dest, int weight);
void free_graph(graph* g);

#endif
