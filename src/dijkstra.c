#include <stdio.h>
#include "dijkstra.h"

// Global variables to share the path with main_GUI.c
int final_path[MAX_NODES];
int final_path_count = 0;

void find_shortest_path(Graph* g, int start, int end) {
    int dist[MAX_NODES], prev[MAX_NODES], visited[MAX_NODES] = {0};
    final_path_count = 0; // Reset count for each call

    // Check for negative weights (Requirement: Invalid input)
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
        final_path[final_path_count++] = start;
        return;
    }

    // Initialize distances
    for (int i = 0; i < g->num_nodes; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[start] = 0;

    // Dijkstra Algorithm Core
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

    // Output Logic
    if (dist[end] == INF) {
        printf("No path found\n");
    } else {
        // Build the path from back to front
        int temp_path[MAX_NODES], count = 0, curr = end;
        while (curr != -1) {
            temp_path[count++] = curr;
            curr = prev[curr];
        }

        // 1. Print result for Milestone 1
        for (int i = count - 1; i >= 0; i--) {
            printf("%d%s", temp_path[i], (i == 0 ? "" : " -> "));
        }
        printf("\n%d\n", dist[end]);

        // 2. Fill the global path array for Milestone 3 GUI
        for (int i = count - 1; i >= 0; i--) {
            final_path[final_path_count++] = temp_path[i];
        }
    }
}
