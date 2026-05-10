#ifndef GRAPH_IO_H
#define GRAPH_IO_H

#include "graph.h"

// Function to handle file reading
int read_graph_from_file(const char* filename, Graph* g, int* start_node, int* end_node);

#endif
