#include <vector>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <set>

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
        int n; // size of graph
        vector<vector<int> > adjMatrix; // adjacency matrix
        set<pair<int, int> > edges; // edges of the graph

        // Constructor
        Graph(int n) {
            this->n = n;
            this->adjMatrix = vector<vector<int> >(n, vector<int>(n, 0));
        }

        // Constructor with adjency matrix
        Graph(int n, vector<vector<int> > adjMatrix) {
            this->n = n;
            this->adjMatrix = adjMatrix;
            // Initialize edges
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (adjMatrix[i][j] != 0) {
                        this->edges.insert(make_pair(i, j));
                    }
                }
            }
        }
        
        // Display the graph
        void display() {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    cout << adjMatrix[i][j] << " ";
                }
                cout << endl;
            }
        }


        // Function to find successors of a vertex
        set<int> getSuccessors(int u);
};

set<int> Graph::getSuccessors(int u) {
    set <int> successors;
    for (int v = 0; v < n; v++) {
        if (adjMatrix[u][v] != 0) {
            successors.insert(v);
        }
    }
    return successors;
}

int main() {
    // Create a simple graph
    vector<vector<int> > graph(4, vector<int>(4, 0));
    Graph g(4, graph);
    g.display();
}

