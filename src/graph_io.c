#include <stdio.h>
#include "graph_io.h"

int read_graph_from_file(const char* filename, Graph* g, int* start_node, int* end_node) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    int num_nodes, num_edges;
    // Get total nodes and edges from first line
    if (fscanf(file, "%d %d", &num_nodes, &num_edges) != 2) {
        fclose(file);
        return 0;
    }

    init_graph(g, num_nodes);

    // Loop to read all edges
    for (int i = 0; i < num_edges; i++) {
        int u, v, w;
        if (fscanf(file, "%d %d %d", &u, &v, &w) == 3) {
            add_edge(g, u, v, w);
        }
    }

    // Last line is for start and end search
    fscanf(file, "%d %d", start_node, end_node);

    fclose(file);
    return 1;
}
