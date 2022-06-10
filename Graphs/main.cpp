#include "g.cpp"
#include "graph.cpp"

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
