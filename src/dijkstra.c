#include <stdio.h>
#include "dijkstra.h"

// Main function to find and print the shortest path
void find_shortest_path(Graph* g, int start, int end) {
    int dist[MAX_NODES], prev[MAX_NODES], visited[MAX_NODES] = {0};

    // Requirement: Check for negative weights (Invalid input)
    for(int i=0; i<g->num_nodes; i++) {
        for(int j=0; j<g->num_nodes; j++) {
            if(g->weights[i][j] < -1) {
                printf("Error: Negative weight found. Invalid input.\n");
                return;
            }
        }
    }

    // Case: Source equals destination
    if (start == end) {
        printf("%d\n0\n", start);
        return;
    }

    // Initialize distances
    for (int i = 0; i < g->num_nodes; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    // Dijkstra Algorithm logic
    for (int i = 0; i < g->num_nodes; i++) {
        int u = -1;
        for (int j = 0; j < g->num_nodes; j++)
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) u = j;

        if (dist[u] == INF) break;
        visited[u] = 1;

        for (int v = 0; v < g->num_nodes; v++) {
            if (g->weights[u][v] != -1) {
                int alt = dist[u] + g->weights[u][v];
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }

    // Case: No path exists
    if (dist[end] == INF) {
        printf("No path found\n");
    } else {
        // Print path with arrows
        int path[MAX_NODES], count = 0, curr = end;
        while (curr != -1) {
            path[count++] = curr;
            curr = prev[curr];
        }
        for (int i = count - 1; i >= 0; i--) {
            printf("%d%s", path[i], (i == 0 ? "" : " -> "));
        }
        // Requirement: Print total weight on a new line
        printf("\n%d\n", dist[end]);
    }
}
