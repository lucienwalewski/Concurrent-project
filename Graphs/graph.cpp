#include <vector>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <set>
#define INF -1

#define MAX 30

using namespace std;

void print_vector(std::vector<int> vector)
{

    for (int i = 0; i < vector.size(); i++)
    {
        std::cout << vector[i] << "\n";
    }
}


void print_set_pair(std::set<std::pair<int, int>> set )
{

    for (auto it = set.begin(); it != set.end(); ++it)
    {
        std::cout << it->first << ' ' <<  it->second <<"\n";
    }
}

void print_vector_of_sets(std::vector<std::set<int>> vector)
{

    for (int i = 0; i < vector.size(); i++)
    {
        std::cout << i << " :\n";
        for (auto it = vector[i].begin(); it != vector[i].end(); ++it)
            std::cout << ' ' << *it;
        std::cout << "\n";
    }
}




void print_set(std::set<int> set){
        for (auto it = set.begin(); it != set.end(); ++it)
    {
        std::cout << *it <<"\n";
    }
}



// Struct edge
struct Edge
{
    int src;
    int dest;
    int weight;
};

// Graph class for weighted directed graphs
// The state of the graph is stored in an adjacency matrix
class Graph
{
public:
    int n;                         // size of graph
    vector<vector<int>> adjMatrix; // adjacency matrix
    set<pair<int, int>> edges;     // edges of the graph

    // Constructor

    Graph()
    {
    }

    Graph(int n)
    {
        this->n = n;
        this->adjMatrix = vector<vector<int>>(n, vector<int>(n, 0));
    }

    // Constructor with adjency matrix
    Graph(int n, vector<vector<int>> adjMatrix)
    {
        this->n = n;
        this->adjMatrix = adjMatrix;
        // Initialize edges
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (adjMatrix[i][j] != 0)
                {
                    this->edges.insert(make_pair(i, j));
                }
            }
        }
    }

    // Display the graph
    void display()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                cout << adjMatrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Function to find successors of a vertex
    set<int> getSuccessors(int u);
};

set<int> Graph::getSuccessors(int u)
{
    set<int> successors;
    for (int v = 0; v < n; v++)
    {
        if (adjMatrix[u][v] != 0)
        {
            successors.insert(v);
        }
    }
    return successors;
}

class Delta_stepping
{
public:
    Delta_stepping(Graph graph, int delta, int source, int thread_num, int n_vertices)
    {
        this->delta = delta;
        this->source = source;
        this->thread_num = thread_num;
        this->n_vertices = n_vertices;
        this->graph = graph;
        for (int v = 0; v < this->n_vertices; v++)
        {
            this->distance.push_back(MAX);
            this->buckets[MAX].insert(v);
        }
        this->distance[source] = 0;

        L = std::vector<std::set<int>>(n_vertices);
        H = std::vector<std::set<int>>(n_vertices);

        for (int i = 0; i < n_vertices; i++)
        {
            for (int j = 0; j < n_vertices; j++)
            {
                int edge = this->graph.adjMatrix[i][j];
                if (edge != INF)
                {
                    if (edge <= delta)
                    {
                        N_l.push_back(std::make_pair(i, j));
                    }
                    else
                    {
                        N_h.push_back(std::make_pair(i, j));
                    }
                }
            }
        }

        // structure of L and H-> [{neighbours of v1}, {neighbours of v2},....]
        for (auto iter : N_l)
        {
            // defining light weight vertex neighbours
            L[iter.first].insert(iter.second);
        }
        for (auto iter : N_h)
        {
            H[iter.first].insert(iter.second);
        }
    }

    int exec()
    {
     this->distance[source] = 0;
     this->buckets[MAX].erase(source);
     this->buckets[0].insert(source);

        int k = 0;

        while (k < MAX)
        {
            std::set<int> R;
            // print_set(buckets[k]);
            while (!this->buckets[k].empty())
            {

                std::set<std::pair<int, int>> Req = find_requests(buckets[k], L);
                R.insert(buckets[k].begin(), buckets[k].end());
                buckets[k].clear();
                relaxRequests(Req);
                
            }
            // print_set(R);

            std::set<std::pair<int, int>> Req = find_requests(R, H);
            relaxRequests(Req);

            int i = 0;
            while (this->buckets[i].empty())
            {
                i++;
            }
            k = i;
            // std::cout << k;

        }

        return 1;
    }

    void relax(int w, int x)
    {
        if (x < distance[w])
        {
            // std::cout << "enter" << std::endl;
            int i = distance[w] / this->delta;
            int j = x / this->delta;
            int remove = *this->buckets[i].find(w);
            this->buckets[i].erase(remove);
            this->buckets[j].insert(w);
            this->distance[w] = x;
            return;
        }
        return;
    }

    std::set<std::pair<int, int>> find_requests(std::set<int> set, std::vector<std::set<int>> kind)
    {
        std::set<std::pair<int, int>> Req;
        for (auto v = set.begin(); v != set.end(); v++)
        {
            // std::cout << "v" << *v << "\n";
            for (auto w = kind[*v].begin(); w != kind[*v].end(); w++)
            {
                // std::cout << "w"<< *w << "\n";

                Req.insert(std::make_pair(*w, this->distance[*v] + graph.adjMatrix[*v][*w]));

                
            }
        }
        return Req;
    }

    void relaxRequests(std::set<std::pair<int, int>> Req)
    {
        for (auto v = Req.begin(); v != Req.end(); v++)
        {
            relax(v->first, v->second);
        }
    }

    int delta, source, thread_num, n_vertices;
    std::vector<int> distance;
    std::set<int> buckets[MAX + 1];

    std::vector<std::set<int>> L;
    std::vector<std::set<int>> H;
    std::vector<std::pair<int, int>> N_l;
    std::vector<std::pair<int, int>> N_h;
    Graph graph;
};



int main()
{

    int n_vertices = 4;

    // Create a simple graph
    // vector<vector<int>> graph(4, vector<int>(4, 0));
    vector<vector<int>> graph{{0, 1, 3, 4},
                              {10, 0, 1, 10},
                              {-1, 10, 0, 1},
                              {-1, -1, 1, 0}};
    Graph g(n_vertices, graph);
    // g.display();

    int delta = 2;
    int source = 3;

    Delta_stepping delta_stepp(g, delta, source, 0, n_vertices);

    delta_stepp.exec();

    // print_vector(delta_stepp.distance);

    // std::cout << "L : \n";
    // print_vector_of_sets(delta_stepp.L);

    // std::cout << "H : \n";
    // print_vector_of_sets(delta_stepp.H);




    print_vector(delta_stepp.distance);

};
