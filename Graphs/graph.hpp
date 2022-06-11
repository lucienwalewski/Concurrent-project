#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>

#define INF std::numeric_limits<int>::max()

typedef int Edge;


class Vertex
{
public:
    int value;
    std::map<Vertex *, Edge> adjacency_list;

    Vertex(int x);

    void link_to(Vertex *vertex, Edge weight);
};


class Graph
{
public:
    int num_vertices;
    std::vector<Vertex *> vertices;
    Graph();

    Graph(int x);

    void add_edge(int from, int to, Edge weight);
    Edge get_edge_value(int from, int to);
    std::vector<int> neighbors(int value);
};


Graph *import_graph(std::string filename, bool directed, bool weighted, bool zero_is_vertex);