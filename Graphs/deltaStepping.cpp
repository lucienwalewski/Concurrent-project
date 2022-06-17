#include "deltaStepping.hpp"

//////////////////////////////////////// Delta_stepping ////////////////////////////////////////

Delta_stepping::Delta_stepping(Graph graph, int delta, int source)
{
    // initialise parameters
    this->n_vertices = graph.num_vertices;
    this->MAX = n_vertices + 99;
    this->buckets = std::vector<std::set<int>>(n_vertices + 100, std::set<int>());
    this->delta = delta;
    this->source = source;
    this->graph = graph;

    // initialise distances to INF, except for source node
    for (int v = 0; v < this->n_vertices; v++)
    {
        this->distance.push_back(INF);
        this->buckets[MAX].insert(v);
    }
    this->distance[source] = 0;

    L = std::vector<std::set<int>>(n_vertices);
    H = std::vector<std::set<int>>(n_vertices);

    // create set of heavy and Low neighbours for each vertex according to the weight of the edge between them.
    for (int vertex = 0; vertex < n_vertices; vertex++)
    {
        for (auto iterator = graph.vertices[vertex]->adjacency_list.begin(); iterator != graph.vertices[vertex]->adjacency_list.end(); iterator++)
        {
            Vertex *destination = iterator->first;
            int weight = iterator->second;
            if (weight != INF)
            {
                if (weight <= delta)
                {
                    L[vertex].insert(destination->value);
                }
                else
                {
                    H[vertex].insert(destination->value);
                }
            }
        }
    }
}

int Delta_stepping::exec()
{
    // insert source in first bucket
    this->distance[source] = 0;
    this->buckets[MAX].erase(source);
    this->buckets[0].insert(source);

    int k = 0;

    while (k < MAX)
    {
        std::set<int> R;
        while (!this->buckets[k].empty())
        {
            // get set of low requests for the present bucket
            std::set<std::pair<int, int>> Req = find_requests(buckets[k], L);
            R.insert(buckets[k].begin(), buckets[k].end());
            buckets[k].clear();
            relaxRequests(Req);
        }
        // get set of heavy request for R
        std::set<std::pair<int, int>> Req = find_requests(R, H);
        relaxRequests(Req);

        // compute new smallest index k such that bucket[k] is not empty
        int i = 0;
        while (this->buckets[i].empty())
        {
            i++;
        }
        k = i;
    }

    return 1;
}

void Delta_stepping::relax(int w, int x)
{
    if (x < distance[w])
    {
        // compute indices i and j, paying attention to INF values
        int i = distance[w] != INF ? distance[w] / this->delta : MAX;
        int j = x != INF ? x / this->delta : MAX;
        int remove = *this->buckets[i].find(w);
        this->buckets[i].erase(remove);
        this->buckets[j].insert(w);
        this->distance[w] = x;
    }
    return;
}

std::set<std::pair<int, int>> Delta_stepping::find_requests(std::set<int> set, std::vector<std::set<int>> kind)
{
    std::set<std::pair<int, int>> Req;
    // find the requests for every node in the set "set"
    for (auto v = set.begin(); v != set.end(); v++)
    {
        for (auto w = kind[*v].begin(); w != kind[*v].end(); w++)
        {
            Req.insert(std::make_pair(*w, this->distance[*v] + graph.vertices[*v]->adjacency_list[graph.vertices[*w]]));
        }
    }
    return Req;
}

void Delta_stepping::relaxRequests(std::set<std::pair<int, int>> Req)
{
    for (auto v = Req.begin(); v != Req.end(); v++)
    {
        relax(v->first, v->second);
    }
}


//////////////////////////////////////// Delta_stepping_parallel ////////////////////////////////////////

Delta_stepping_parallel::Delta_stepping_parallel(Graph graph, int delta, int source, int thread_num)
{
    this->n_vertices = graph.num_vertices;
    this->MAX = n_vertices + 99;
    this->buckets = std::vector<std::set<int>>(n_vertices + 100, std::set<int>());
    this->delta = delta;
    this->source = source;
    // we make sure that we have more vertex than thread.
    thread_num = n_vertices > thread_num ? thread_num : n_vertices;
    this->thread_num = thread_num;
    this->graph = graph;

    // as before, we initialise the distances to infinity
    for (int v = 0; v < this->n_vertices; v++)
    {
        this->distance.push_back(INF);
        this->buckets[MAX].insert(v);
    }
    this->distance[source] = 0;

    L = std::vector<std::set<int>>(n_vertices);
    H = std::vector<std::set<int>>(n_vertices);

    // create set of neighbours for each vertex
    for (int vertex = 0; vertex < n_vertices; vertex++)
    {
        for (auto iterator = graph.vertices[vertex]->adjacency_list.begin(); iterator != graph.vertices[vertex]->adjacency_list.end(); iterator++)
        {
            Vertex *destination = iterator->first;
            int weight = iterator->second;
            if (weight != INF)
            {
                if (weight <= delta)
                {
                    L[vertex].insert(destination->value);
                }
                else
                {
                    H[vertex].insert(destination->value);
                }
            }
        }
    }

    // assign the vertices to the threads at random.
    this->thread_assignment = std::vector<std::set<int>>(thread_num);
    for (int vertex = 0; vertex < n_vertices; vertex++)
    {
        int thread = rand() % thread_num;
        this->thread_assignment[thread].insert(vertex);
    }
}

int Delta_stepping_parallel::exec()
{
    this->distance[source] = 0;
    this->buckets[MAX].erase(source);
    this->buckets[0].insert(source);

    int k = 0;

    while (k < MAX)
    {
        std::set<int> R;
        while (!this->buckets[k].empty())
        {
            // find the requests in parallel
            std::set<std::pair<int, int>> Req = find_requests_parallel(buckets[k], L);
            R.insert(buckets[k].begin(), buckets[k].end());
            buckets[k].clear();
            // relax the requests in parallel
            relaxRequests_parallel(Req);
        }

        std::set<std::pair<int, int>> Req = find_requests_parallel(R, H);
        relaxRequests_parallel(Req);

        // compute k, the minimum index such that bucket[k] is not empty (as before)
        int i = 0;
        while (this->buckets[i].empty())
        {
            i++;
        }
        k = i;
    }

    return 1;
}

void Delta_stepping_parallel::relax(int w, int x)
{
    // same function as in the sequential version

    if (x < distance[w])
    {
        int i = distance[w] != INF ? distance[w] / this->delta : MAX;
        int j = x != INF ? x / this->delta : MAX;
        int remove = *this->buckets[i].find(w);
        this->buckets[i].erase(remove);
        this->buckets[j].insert(w);
        this->distance[w] = x;
        return;
    }
    return;
}

void Delta_stepping_parallel::find_requests(std::set<int> set, std::vector<std::set<int>> kind, std::set<std::pair<int, int>> &Req_t)
{
    // same function as in the sequential version
    for (auto v = set.begin(); v != set.end(); v++)
    {
        for (auto w = kind[*v].begin(); w != kind[*v].end(); w++)
        {

            Req_t.insert(std::make_pair(*w, this->distance[*v] + graph.vertices[*v]->adjacency_list[graph.vertices[*w]]));
        }
    }
    return;
}

std::set<std::pair<int, int>> Delta_stepping_parallel::find_requests_parallel(std::set<int> set, std::vector<std::set<int>> kind)
{
    std::thread workers[thread_num];
    std::set<std::pair<int, int>> Req;
    std::set<int> set_t[thread_num];
    std::set<std::pair<int, int>> Req_t[thread_num];

    // Create the partition S_t of our set, where S_t is the intersection between S and the set of edges assigned to thread t.
    // To do this, we compute the intersection of our original set with the set of vertices assigned to S
    for (int t = 0; t < thread_num; t++)
    {

        for (auto &v : set)
        {
            if (thread_assignment[t].find(v) != thread_assignment[t].end())
            {
                set_t[t].insert(v);
            }
        }
    }

    // execute the function find requests in parallel
    for (int t = 0; t < thread_num; t++)
    {
        workers[t] = std::thread(&Delta_stepping_parallel::find_requests, this, std::ref(set_t[t]), kind, std::ref(Req_t[t]));
    }

    // join the threads
    for (int t = 0; t < thread_num; t++)
    {
        if (workers[t].joinable())
        {
            workers[t].join();
        };
    }

    // merge all the subrequests into a single list
    for (int t = 0; t < thread_num; t++)
    {
        Req.insert(Req_t[t].begin(), Req_t[t].end());
    }

    return Req;
}

void Delta_stepping_parallel::relaxRequests(std::set<std::pair<int, int>> Req)
{
    for (auto v = Req.begin(); v != Req.end(); v++)
    {
        relax(v->first, v->second);
    }
}

void Delta_stepping_parallel::relaxRequests_parallel(std::set<std::pair<int, int>> Req)
{

    std::set<std::pair<int, int>> Req_t[thread_num];
    std::thread workers[thread_num];
    // compute the sets S_r for every r, where S_r is the partition of S containing all the requests which destination is a vertex assigned to thread r.
    // As before, this is done by computing intersections
    for (int t = 0; t < thread_num; t++)
    {

        for (auto &v : Req)
        {
            if (thread_assignment[t].find(v.first) != thread_assignment[t].end())
            {
                Req_t[t].insert(v);
            }
        }
    }
    // execute relaxRequests in parallel
    for (int t = 0; t < thread_num; t++)
    {
        workers[t] = std::thread(&Delta_stepping_parallel::relaxRequests, this, Req_t[t]);
    }
    for (int t = 0; t < thread_num; t++)
    {
        if (workers[t].joinable())
        {
            workers[t].join();
        };
    }
}
