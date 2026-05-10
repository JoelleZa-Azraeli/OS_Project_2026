#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define INF INT_MAX

// Function to print the path using the parent array
void printPath(int parent[], int j) {
    if (parent[j] == -1) {
        printf("%d", j);
        return;
    }
    printPath(parent, parent[j]);
    printf(" -> %d", j);
}

void dijkstra(int n, int graph[n][n], int startNode, int endNode) {
    int dist[n];      // Shortest distance from startNode to i
    bool visited[n];  // visited[i] will be true if vertex i is included in shortest path tree
    int parent[n];    // Array to store the shortest path tree

    // Initialize all distances as INFINITE and visited[] as false
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = false;
        parent[i] = -1;
    }

    // Distance of source vertex from itself is always 0
    dist[startNode] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < n - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not yet processed
        int min = INF, u = -1;

        for (int v = 0; v < n; v++) {
            if (!visited[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }
        }

        // If we can't find a reachable node, break
        if (u == -1) break;

        visited[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex
        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] != INF && dist[u] != INF 
                && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    // Handle Output Requirements
    if (startNode == endNode) {
        printf("%d\n0\n", startNode);
    } else if (dist[endNode] == INF) {
        printf("No path found\n");
    } else {
        printPath(parent, endNode);
        printf("\n%d\n", dist[endNode]);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file_name>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    int N, M;
    if (fscanf(fp, "%d %d", &N, &M) != 2) return 1;

    // Adjacency Matrix
    int graph[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }

    for (int i = 0; i < M; i++) {
        int u, v, w;
        if (fscanf(fp, "%d %d %d", &u, &v, &w) == 3) {
            if (w < 0) { // Error handling for negative weights
                printf("Error: Negative weights not allowed\n");
                fclose(fp);
                return 1;
            }
            graph[u][v] = w;
        }
    }

    int start_node, end_node;
    if (fscanf(fp, "%d %d", &start_node, &end_node) != 2) {
        fclose(fp);
        return 1;
    }
    fclose(fp);

    dijkstra(N, graph, start_node, end_node);

    return 0;
}
