#include "dijkstra.hpp"

//////////////////////////////////////// DIJKSTRA SEQUENTIAL ////////////////////////////////////////

std::vector<int> dijkstra(Graph *graph, int source) {
    // Vector of distances from source to every vertex
    std::vector<int> dist(graph->num_vertices, INF);
    dist[source] = 0;

    // Initialise priority queue
    std::set<std::pair<int, int>> queue;
    queue.insert(std::pair<int, int>(dist[source], source));

    // While queue is not empty
    while (!queue.empty()) {
        // Get distance, vertex pair with minimum distance
        auto[d, u] = *queue.begin();
        queue.erase(queue.begin());

        // Relax all edges from u
        for (auto&[v, e] : graph->vertices[u]->adjacency_list) {
            if (d + e < dist[v->value]) {
                // Remove old edge from queue
                queue.erase(std::pair<int, int>(dist[v->value], v->value));
                // Update distance
                dist[v->value] = d + e;
                // Add new edge to queue
                queue.insert(std::pair<int, int>(dist[v->value], v->value));
            }
        }
    }
    return dist;
}


//////////////////////////////////////// DIJKSTRA PARALLEL USING setList ////////////////////////////////////////

void relax_dijkstra(Graph *graph, int u, int d, std::vector<int> &dist, SetList &queue, int t, int thread_num) {
    std::map<Vertex *, Edge>::iterator it = graph->vertices[u]->adjacency_list.begin();
    // std::cout << "start relax_dijkstra" << std::endl;
    int it_position = t; // Keeps track of the position of the iterator
    if (t < graph->vertices[u]->adjacency_list.size()) {
        advance(it, t);
    } else {
        return;
    }
    while (it_position < graph->vertices[u]->adjacency_list.size()) {
        auto[v, e] = *it;
        // std::cout << " " << std::endl;
        // std::cout << "vertex:: " << v->value << std::endl;
        // std::cout << "weight:: " << e << std::endl;
        if (d + e < dist[v->value]) {
            // Remove old edge from queue
            // std::cout << "remove old edge from queue" << std::endl;
            queue.remove(std::pair<int, int>(dist[v->value], v->value));
            // std::cout << "remove old edge from queue" << std::endl;
            // Update distance
            // std::cout << "update distance" << std::endl;
            dist[v->value] = d + e;
            // std::cout << "update distance" << std::endl;
            // Add new edge to queue
            // std::cout << "add new edge to queue" << std::endl;
            queue.add(std::pair<int, int>(dist[v->value], v->value));
            // std::cout << "add new edge to queue" << std::endl;
        }
        if (it_position + thread_num < graph->vertices[u]->adjacency_list.size()) {
            // std::cout << "advance" << std::endl;
            advance(it, thread_num);
            // std::cout << "finished advance" << std::endl;
            it_position += thread_num;
        } else {
            return;
        }
    }
}


std::vector<int> dijkstra_parallel(Graph *graph, int source, int thread_num) {
    // Vector of distances from source to every vertex
    std::vector<int> dist(graph->num_vertices, INF);
    dist[source] = 0;

    // Initialise priority queue with thread-safe SetList
    SetList queue;
    // queue.print();
    queue.add(std::pair<int, int>(dist[source], source));

    // Initialise workers
    std::thread workers[thread_num - 1];

    // std::cout << "Dijkstra parallel" << std::endl;
    // While queue is not empty
    while (!queue.empty()) {
        // Get distance, vertex pair with minimum distance
        // queue.print();
        auto[d, u] = queue.pop();

        // Relax all edges from u in parallel
        for (int t = 0; t < thread_num - 1; t++) {
            // std::cout << "launch worker" << std::endl;
            workers[t] = std::thread(&relax_dijkstra, graph, u, d, std::ref(dist), std::ref(queue), t, thread_num);
        }
        relax_dijkstra(graph, u, d, dist, queue, thread_num - 1, thread_num);
        // std::cout << "finished relax" << std::endl;
        // Join the threads
        for (int t = 0; t < thread_num - 1; t++) {
            workers[t].join();
        }
    }
    return dist;
}


//////////////////////////////////////// DIJKSTRA PARALLEL USING BST ////////////////////////////////////////

void relax_dijkstra_BST(Graph *graph, int u, int d, std::vector<int> &dist, FineBST &tree, int t, int thread_num) {
    std::map<Vertex *, Edge>::iterator it = graph->vertices[u]->adjacency_list.begin();
    // std::cout << "start relax_dijkstra" << std::endl;
    int it_position = t; // Keeps track of the position of the iterator
    if (t < graph->vertices[u]->adjacency_list.size()) {
        advance(it, t);
    } else {
        return;
    }
    while (it_position < graph->vertices[u]->adjacency_list.size()) {
        auto[v, e] = *it;
        // std::cout << " " << std::endl;
        // std::cout << "vertex:: " << v->value << std::endl;
        // std::cout << "weight:: " << e << std::endl;
        if (d + e < dist[v->value]) {
            // Remove old edge from queue
            // std::cout << "remove old edge from queue" << std::endl;
            tree.remove(std::pair<int, int>(dist[v->value], v->value));
            // std::cout << "remove old edge from queue" << std::endl;
            // Update distance
            // std::cout << "update distance" << std::endl;
            dist[v->value] = d + e;
            // std::cout << "update distance" << std::endl;
            // Add new edge to queue
            // std::cout << "add new edge to queue" << std::endl;
            tree.add(std::pair<int, int>(dist[v->value], v->value));
            // std::cout << "add new edge to queue" << std::endl;
        }
        if (it_position + thread_num < graph->vertices[u]->adjacency_list.size()) {
            // std::cout << "advance" << std::endl;
            advance(it, thread_num);
            // std::cout << "finished advance" << std::endl;
            it_position += thread_num;
        } else {
            return;
        }
    }
}


std::vector<int> dijkstra_parallel_BST(Graph *graph, int source, int thread_num) {
    // Vector of distances from source to every vertex
    std::vector<int> dist(graph->num_vertices, INF);
    dist[source] = 0;

    // Initialise priority queue with thread-safe BST
    FineBST tree;
    // queue.print();
    tree.add(std::pair<long, long>(dist[source], source));

    // Initialise workers
    std::thread workers[thread_num - 1];

    // std::cout << "Dijkstra parallel BST" << std::endl;
    // While queue is not empty
    while (!tree.empty()) {
        // Get distance, vertex pair with minimum distance
        // std::cout << "pop" << std::endl;
        auto[d, u] = tree.pop();

        // Relax all edges from u in parallel
        for (int t = 0; t < thread_num - 1; t++) {
            // std::cout << "launch worker" << std::endl;
            workers[t] = std::thread(&relax_dijkstra_BST, graph, u, d, std::ref(dist), std::ref(tree), t, thread_num);
        }
        relax_dijkstra_BST(graph, u, d, dist, tree, thread_num - 1, thread_num);
        // std::cout << "finished relax" << std::endl;
        // Join the threads
        for (int t = 0; t < thread_num - 1; t++) {
            workers[t].join();
        }
    }
    return dist;
}
