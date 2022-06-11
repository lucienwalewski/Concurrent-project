#pragma once

#include "setList.hpp"
#include "graph.hpp"

#include <thread>

std::vector<int> dijkstra(Graph *graph, int source);

void relax_dijkstra(Graph *graph, int u, int d, std::vector<int> &dist, SetList &queue, int t, int thread_num);

std::vector<int> dijkstra_parallel(Graph *graph, int source, int thread_num);