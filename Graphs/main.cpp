#include "graph.cpp"



// delta stepping algorithm as presented in this paper : https://www.researchgate.net/publication/222719985_Delta-stepping_a_parallelizable_shortest_path_algorithm
class Delta_stepping
{
public:
    Delta_stepping(Graph graph, int delta, int source)
    {
        // initialise parameters
        this->n_vertices = graph.num_vertices;
        this->MAX = n_vertices + 99;
        this->buckets = std::vector<std::set<int>>(n_vertices + 100, std::set<int>());
        this->delta = delta;
        this->source = source;
        this->n_vertices = n_vertices;
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

    int exec()
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

    void relax(int w, int x)
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

    std::set<std::pair<int, int>> find_requests(std::set<int> set, std::vector<std::set<int>> kind)
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

    void relaxRequests(std::set<std::pair<int, int>> Req)
    {
        for (auto v = Req.begin(); v != Req.end(); v++)
        {
            relax(v->first, v->second);
        }
    }

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
    Delta_stepping_parallel(Graph graph, int delta, int source, int thread_num)
    {
        this->n_vertices = graph.num_vertices;
        this->MAX = n_vertices + 99;
        this->buckets = std::vector<std::set<int>>(n_vertices + 100, std::set<int>());
        this->delta = delta;
        this->source = source;
        // we make sure that we have more vertex than thread.
        thread_num = n_vertices > thread_num ? thread_num : n_vertices;
        this->thread_num = thread_num;

        this->n_vertices = n_vertices;
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

    int exec()
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

    void relax(int w, int x)
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

    void find_requests(std::set<int> set, std::vector<std::set<int>> kind, std::set<std::pair<int, int>> &Req_t)
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

    std::set<std::pair<int, int>> find_requests_parallel(std::set<int> set, std::vector<std::set<int>> kind)
    {
        std::thread workers[thread_num];
        std::set<std::pair<int, int>> Req;
        std::set<int> set_t[thread_num];
        std::set<std::pair<int, int>> Req_t[thread_num];

        // Create the partition S_t of our set, where S_t is the intersection between S and the set of edge starting from a vertex assigned to thread t.
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

    void relaxRequests(std::set<std::pair<int, int>> Req)
    {
        for (auto v = Req.begin(); v != Req.end(); v++)
        {
            relax(v->first, v->second);
        }
    }

    void relaxRequests_parallel(std::set<std::pair<int, int>> Req)
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

    int delta, source, thread_num, n_vertices;
    std::vector<int> distance;
    std::vector<std::set<int>> buckets;
    int MAX;
    std::vector<std::set<int>> L;
    std::vector<std::set<int>> H;
    std::vector<std::set<int>> thread_assignment;

    Graph graph;
};

//////////////////////////////////////// DIJKSTRA SEQUENTIAL ////////////////////////////////////////
std::vector<int> dijkstra(Graph *graph, int source)
{
    std::vector<int> dist(graph->num_vertices, INF);
    std::set<std::pair<int, int>> s;

    dist[source] = 0;
    s.insert(std::pair<int, int>(dist[source], source));

    while (!s.empty())
    {
        std::pair<int, int> now = *s.begin();
        s.erase(s.begin());

        int d = now.first;
        int v = now.second;

        for (std::map<Vertex *, Edge>::iterator it = graph->vertices[v]->adjacency_list.begin(); it != graph->vertices[v]->adjacency_list.end(); it++)
        {
            if (d + it->second < dist[it->first->value])
            {
                s.erase(std::pair<int, int>(dist[it->first->value], it->first->value));
                dist[it->first->value] = d + it->second;
                s.insert(std::pair<int, int>(dist[it->first->value], it->first->value));
            }
        }
    }
    return dist;
}

void print_vector(std::vector<int> vector)
{

    for (int i = 0; i < vector.size(); i++)
    {
        std::cout << vector[i] << "\n";
    }
}

void test(Graph *g1, int delta, int source, int threads)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Running Dijkstra ..." << std::endl;
    std::vector<int> result = dijkstra(g1, source);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    Delta_stepping delta_stepp(*g1, delta, source);
    std::cout << "Running sequential delta-stepping ..." << std::endl;
    ;
    start = std::chrono::high_resolution_clock::now();
    delta_stepp.exec();
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    Delta_stepping_parallel delta_stepp_par(*g1, delta, source, threads);
    std::cout << "Running parallel delta-stepping ..." << std::endl;
    ;
    start = std::chrono::high_resolution_clock::now();
    delta_stepp_par.exec();
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    std::cout << "Checking results ...";

    int errors = 0;
    for (int i = 0; i < delta_stepp_par.n_vertices; i++)
    {
        if (delta_stepp_par.distance[i] != result[i] or delta_stepp_par.distance[i] != result[i])
        {
            std::cout << "error when computing shortest path to " << i << std::endl;
            std::cout << "parallel delta stepping gives " << delta_stepp_par.distance[i] << std::endl;
            std::cout << "sequential delta stepping gives " << delta_stepp.distance[i] << std::endl;
            std::cout << "Dijkstra gives " << result[i] << std::endl;
            errors += 1;
        }
    }
    std::cout << errors << " errors on " << delta_stepp_par.n_vertices << " tests.";
}

int main(int argc, char** argv)
{   
    if (argc!=4) {
        std::cout << "not enough arguments were given" << std::endl;
        return 1;
    }
    int source = std::stoi(argv[1]);
    int threads = std::stoi(argv[2]);
    int delta = std::stoi(argv[3]);
    std::cout << "Creating Graph" << std::endl;
    Graph *g1 = import_graph("test_graph.txt", true, true, false);
    test(g1, delta, source, threads);

    return 0;
}
