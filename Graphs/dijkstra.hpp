#pragma once

#include "setList.hpp"
#include "graph.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <set>
#include <map>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>

std::vector<int> dijkstra(Graph *graph, int source);

void relax_dijkstra(Graph *graph, int u, int d, std::vector<int> &dist, SetList &queue, int t, int thread_num);

std::vector<int> dijkstra_parallel(Graph *graph, int source, int thread_num);