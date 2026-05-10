#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NODES 15
#define INF 999999

// Simple structure for the graph matrix
typedef struct {
    int num_nodes;
    int weights[MAX_NODES][MAX_NODES];
} Graph;

void init_graph(Graph* g, int nodes);
void add_edge(Graph* g, int src, int dst, int weight);

#endif
