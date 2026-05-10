#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

// Milestone 1 function
void find_shortest_path(Graph* g, int start, int end);

// Milestone 3: Shared variables for the GUI animation
extern int final_path[MAX_NODES];
extern int final_path_count;

#endif
