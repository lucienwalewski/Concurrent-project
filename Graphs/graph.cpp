#include <vector>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

// Struct edge
struct Edge {
    int src;
    int dest;
    int weight;
};

// Graph class for weighted directed graphs
// The state of the graph is stored in an adjacency matrix
class Graph {
    public:
        int n; // number of vertices
        vector<vector<int> > adjMatrix; // adjacency matrix

        // Constructor
        Graph(int n) {
            this->n = n;
            this->adjMatrix = vector<vector<int> >(n, vector<int>(n, 0));
        }
};

int main() {
    Graph g(5);
}

