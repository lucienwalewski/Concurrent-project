#pragma once

#include "setList.hpp"
#include "BST.hpp"
#include "graph.hpp"

#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>


std::vector<int> dijkstra(Graph *graph, int source);

void relax_dijkstra(Graph *graph, int u, int d, std::vector<int> &dist, SetList &queue, int t, int thread_num);

std::vector<int> dijkstra_parallel(Graph *graph, int source, int thread_num);

void relax_dijkstra_BST(Graph *graph, int u, int d, std::vector<int> &dist, FineBST &tree, int t, int thread_num);

std::vector<int> dijkstra_parallel_BST(Graph *graph, int source, int thread_num);