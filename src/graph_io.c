#include <stdio.h>
#include <string.h>
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

int read_graph_with_travelers(const char* filename, Graph* g, Traveler* travelers, int* num_travelers) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[256];
    int num_nodes = 0, num_edges = 0;

    // Find first data line (skip # comments and blank lines)
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        if (sscanf(line, "%d %d", &num_nodes, &num_edges) == 2) break;
    }
    if (num_nodes == 0) { fclose(file); return 0; }

    init_graph(g, num_nodes);

    int edges_read = 0;
    while (edges_read < num_edges && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        int u, v, w;
        if (sscanf(line, "%d %d %d", &u, &v, &w) == 3) {
            add_edge(g, u, v, w);
            edges_read++;
        }
    }

    // Read traveler count
    *num_travelers = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        int n;
        if (sscanf(line, "%d", &n) == 1) { *num_travelers = n; break; }
    }

    int t = 0;
    while (t < *num_travelers && fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        if (sscanf(line, "%d %d", &travelers[t].src, &travelers[t].dst) == 2) t++;
    }
    *num_travelers = t;

    fclose(file);
    return 1;
}
