#include <stdio.h>
#include "graph_io.h"
#include "dijkstra.h"

int main(int argc, char* argv[]) {
    // Check if filename was provided in terminal
    if (argc < 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    Graph g;
    int start, end;

    // Run the process
    if (read_graph_from_file(argv[1], &g, &start, &end)) {
        find_shortest_path(&g, start, end);
    } else {
        printf("Error reading file.\n");
    }

    return 0;
}
