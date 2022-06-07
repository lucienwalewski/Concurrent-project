#include <vector>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <set>
#include <map>
#include <limits>
#define INF std::numeric_limits<int>::max()

typedef int Edge;

class Vertex
{
public:
    int value;
    std::map<Vertex *, Edge> adjacency_list;
    
    Vertex(int x){
        this->value = x;
    }

    void link_to(Vertex *vertex, Edge weight);
};

void Vertex::link_to(Vertex *vertex, Edge weight) {
    std::map<Vertex *, Edge>::iterator it = this->adjacency_list.find(vertex);
    if (it == this->adjacency_list.end()) {
        this->adjacency_list.insert(std::pair<Vertex *, Edge>(vertex, weight));
    } else {
        std::cout << "Already existing edge between vertex " << this->value << " and vertex " << vertex->value << "." << std::endl;
        // If the vertex is already in the adjacency_list, it does nothing.
    }
}




class Graph
{
public:
    int num_vertices;
    std::vector<Vertex *> vertices;
    
    Graph(int x) {
        this->num_vertices = x;
        Vertex *vertex;
        for (int i = 0; i < this->num_vertices; i++){
            vertex = new Vertex(i);
            this->vertices.push_back(vertex);
        }
    };

    void add_edge(int from, int to, Edge weight);
    Edge get_edge_value(int from, int to);
    std::vector<int> neighbors(int value);
};

void Graph::add_edge(int from, int to, Edge weight) {
    this->vertices[from]->link_to(this->vertices[to], weight);
}

Edge Graph::get_edge_value(int from, int to) {
    std::map<Vertex *, Edge>::iterator it = this->vertices[from]->adjacency_list.find(vertices[to]);
    if (it != this->vertices[from]->adjacency_list.end()) {
        return it->second;
    } else {
        std::cout << "No existing edge between vertex " << from << " and vertex " << to << "." << std::endl;
        return INF;
    }
}

std::vector<int> Graph::neighbors(int value){
    std::vector<int> list_neighbors;
    for (std::map<Vertex *, Edge>::iterator it = this->vertices[value]->adjacency_list.begin(); it != this->vertices[value]->adjacency_list.end(); it++) {
        list_neighbors.push_back(it->first->value);
    }
    return list_neighbors;
}




std::vector<int> dijkstra(Graph *graph, int source) {
    std::vector<int> dist(graph->num_vertices, INF);
    std::set<std::pair<int, int> > s;

    dist[source] = 0;
    s.insert(std::pair<int, int>(dist[source], source));

    while (!s.empty()) {
        std::pair<int, int> now = *s.begin();
        s.erase(s.begin());

        int d = now.first;
        int v = now.second;

        for (std::map<Vertex *, Edge>::iterator it = graph->vertices[v]->adjacency_list.begin(); it != graph->vertices[v]->adjacency_list.end(); it++) {
            if (d + it->second < dist[it->first->value]) {
                s.erase(std::pair<int, int>(dist[it->first->value], it->first->value));
                dist[it->first->value] = d + it->second;
                s.insert(std::pair<int, int>(dist[it->first->value], it->first->value));
            }
        }
    }
    return dist;
}



int main()
{
    std::cout << "I love this project!" <<  std::endl;

    Graph *g = new Graph(3);
    std::cout << g->vertices[0]->value << std::endl;
    std::cout << g->vertices[1]->value << std::endl;
    std::cout << g->vertices[2]->value << std::endl;
    g->add_edge(0, 1, 3);
    g->add_edge(0, 1, 3);
    g->add_edge(1, 2, 2);
    g->add_edge(0, 2, 7);
    g->get_edge_value(1, 0);
    std::cout << INF << std::endl;

    std::vector<int> a = dijkstra(g, 0);
    std::cout << a[0] << " " << a[1] << " " << a[2] << std::endl;
    a = dijkstra(g, 2);
    std::cout << a[0] << " " << a[1] << " " << a[2] << std::endl;
    return 0;
}