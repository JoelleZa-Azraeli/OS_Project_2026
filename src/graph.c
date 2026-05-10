#include "graph.h"

// Set all distances to -1 at the start
void init_graph(Graph* g, int nodes) {
    g->num_nodes = nodes;
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            g->weights[i][j] = -1;
        }
    }
}

// Add a new connection between nodes
void add_edge(Graph* g, int src, int dst, int weight) {
    if (src >= 0 && src < g->num_nodes && dst >= 0 && dst < g->num_nodes) {
        g->weights[src][dst] = weight;
    }
}
