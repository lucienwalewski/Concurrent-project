#pragma once

#include "graph.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <fstream>
#include <sstream>

// delta stepping algorithm as presented in this paper : https://www.researchgate.net/publication/222719985_Delta-stepping_a_parallelizable_shortest_path_algorithm
class Delta_stepping
{
public:
    Delta_stepping(Graph graph, int delta, int source);

    int exec();

    void relax(int w, int x);

    std::set<std::pair<int, int>> find_requests(std::set<int> set, std::vector<std::set<int>> kind);

    void relaxRequests(std::set<std::pair<int, int>> Req);

    int delta, source, n_vertices;
    std::vector<int> distance;
    std::vector<std::set<int>> buckets;
    int MAX;
    std::vector<std::set<int>> L;
    std::vector<std::set<int>> H;
    Graph graph;
};

// Lock free parallelisation of our sequential algorithm,
// as presented in this paper : https://old.insight-centre.org/sites/default/files/publications/engineering_a_parallel_-stepping_algorithm.pdf
// Each vertex is randomly assigned to a thread index t.
// We then find the requests running the find request function in parallel on every partition S_t of the set S,
// Where S_t is the intersection between S and the set of edge starting from a vertex assigned to thread t.

// We then relax all the requests in parrallel, executing the function relaxRequest on every set S_r, where S_r
// is the partition of S containing all the requests which destination is a vertex assigned to thread r.

// This particular partitionning ensures no race condition.
class Delta_stepping_parallel
{
public:
    Delta_stepping_parallel(Graph graph, int delta, int source, int thread_num);

    int exec();

    void relax(int w, int x);

    void find_requests(std::set<int> set, std::vector<std::set<int>> kind, std::set<std::pair<int, int>> &Req_t);

    std::set<std::pair<int, int>> find_requests_parallel(std::set<int> set, std::vector<std::set<int>> kind);

    void relaxRequests(std::set<std::pair<int, int>> Req);

    void relaxRequests_parallel(std::set<std::pair<int, int>> Req);

    int delta, source, thread_num, n_vertices;
    std::vector<int> distance;
    std::vector<std::set<int>> buckets;
    int MAX;
    std::vector<std::set<int>> L;
    std::vector<std::set<int>> H;
    std::vector<std::set<int>> thread_assignment;
    Graph graph;
};