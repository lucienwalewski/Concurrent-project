#include "graph.hpp"

//////////////////////////////////////// VERTEX CLASS ////////////////////////////////////////

Vertex::Vertex(int x)
{
    this->value = x;
}

void Vertex::link_to(Vertex *vertex, Edge weight)
{
    std::map<Vertex *, Edge>::iterator it = this->adjacency_list.find(vertex);
    if (it == this->adjacency_list.end())
    {
        this->adjacency_list.insert(std::pair<Vertex *, Edge>(vertex, weight));
    }
    else
    {
        std::cout << "Already existing edge between vertex " << this->value << " and vertex " << vertex->value << "." << std::endl;
        std::cout << "Weight: " << weight << std::endl;
        // If the vertex is already in the adjacency_list, it does nothing.
    }
}




//////////////////////////////////////// GRAPH CLASS ////////////////////////////////////////

Graph::Graph(){};

Graph::Graph(int x)
{
    this->num_vertices = x;
    Vertex *vertex;
    for (int i = 0; i < this->num_vertices; i++)
    {
        vertex = new Vertex(i);
        this->vertices.push_back(vertex);
    }
}


void Graph::add_edge(int from, int to, Edge weight)
{
    // std::cout << from << " " << to << " " << weight << std::endl;
    this->vertices[from]->link_to(this->vertices[to], weight);
}

Edge Graph::get_edge_value(int from, int to)
{
    std::map<Vertex *, Edge>::iterator it = this->vertices[from]->adjacency_list.find(vertices[to]);
    if (it != this->vertices[from]->adjacency_list.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "No existing edge between vertex " << from << " and vertex " << to << "." << std::endl;
        return INF;
    }
}

std::vector<int> Graph::neighbors(int value)
{
    std::vector<int> list_neighbors;
    for (std::map<Vertex *, Edge>::iterator it = this->vertices[value]->adjacency_list.begin(); it != this->vertices[value]->adjacency_list.end(); it++)
    {
        list_neighbors.push_back(it->first->value);
    }
    return list_neighbors;
}




//////////////////////////////////////// IMPORT GRAPH ////////////////////////////////////////

Graph *import_graph(std::string filename, bool directed, bool weighted, bool zero_is_vertex)
{
    filename = "../text_files/" + filename;
    const char *input = filename.c_str();
    std::ifstream file(input);
    Graph *graph;
    int num_vertices, num_edges;
    int u, v;
    // int max;
    if (file >> num_vertices >> num_edges)
    {
        graph = new Graph(num_vertices);
        // std::cout << "Graph created" << std::endl;
        if (weighted)
        {
            int w;
            for (int i = 0; i < num_edges; i++)
            {
                file >> u >> v >> w;
                // std::cout << u << v << w << std::endl;
                if (zero_is_vertex)
                {
                    graph->add_edge(u, v, w);
                }
                else
                {
                    graph->add_edge(u - 1, v - 1, w);
                }
                if (!directed)
                {
                    if (zero_is_vertex)
                    {
                        graph->add_edge(v, u, w);
                    }
                    else
                    {
                        graph->add_edge(v - 1, u - 1, w);
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < num_edges; i++)
            {
                file >> u >> v;
                /*
                if (u > max) {
                    max = u;
                }
                if (v > max) {
                    max = v;
                }
                */
                // std::cout << u << " " << v << std::endl;
                if (zero_is_vertex)
                {
                    graph->add_edge(u, v, 1);
                }
                else
                {
                    graph->add_edge(u - 1, v - 1, 1);
                }
                if (!directed)
                {
                    if (zero_is_vertex)
                    {
                        graph->add_edge(v, u, 1);
                    }
                    else
                    {
                        graph->add_edge(v - 1, u - 1, 1);
                    }
                }
            }
        }
    }
    // std::cout << max << std::endl;
    file.close();
    return graph;
}
