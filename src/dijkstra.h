#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

// Milestone 1: prints path to terminal, also fills final_path globals
void find_shortest_path(Graph* g, int start, int end);

// Milestones 4-6: returns path length (0 = no path), fills path_out[0..n-1]
int compute_path(Graph* g, int start, int end, int* path_out);

// Milestone 3: Shared variables for the GUI animation
extern int final_path[MAX_NODES];
extern int final_path_count;

#endif
