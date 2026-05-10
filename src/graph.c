#include "graph.h"
#include <stdlib.h>

graph* create_graph(int node_num) {
    graph* g = malloc(sizeof(graph));
    g->node_num = node_num;
    g->adjacency_list = calloc(node_num, sizeof(edge*));
    return g;
}

void add_edge(graph* g, int src, int dest, int weight) {
    edge* new_node = malloc(sizeof(edge));
    new_node->dest = dest;
    new_node->weight = weight;
    new_node->next = g->adjacency_list[src];
    g->adjacency_list[src] = new_node;
}

void free_graph(graph* g) {
    for (int i = 0; i < g->node_num; i++) {
        edge* curr = g->adjacency_list[i];
        while (curr) {
            edge* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(g->adjacency_list);
    free(g);
}
