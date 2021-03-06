#include "deltaStepping.hpp"
#include "dijkstra.hpp"

#include <chrono>


void print_vector(std::vector<int> vector)
{
    for (int i = 0; i < vector.size(); i++)
    {
        std::cout << vector[i] << "\n";
    }
}


void test(Graph *g1, int delta, int source, int threads)
{

    std::cout << "Running sequential Dijkstra ..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> result_seq = dijkstra(g1, source);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    std::cout << "Running parallel Dijkstra (setList) ..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> result_par_setList = dijkstra_parallel(g1, source, threads);
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << time.count() << "ms" << std::endl;

    std::cout << "Running parallel Dijkstra (BST) ..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> result_par_BST = dijkstra_parallel_BST(g1, source, threads);
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
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
    
    std::cout << "Checking results ..." << std::endl;

    int errors = 0;
    
    for (int i = 0; i < result_seq.size(); i++)
    {
        if (result_seq[i] != result_par_setList[i] || result_seq[i] != result_par_BST[i]) {
            std::cout << "error when computing shortest path to " << i << std::endl;
            std::cout << "Dijkstra gives " << result_seq[i] << std::endl;
            std::cout << "parallel Dijkstra (setList) gives " << result_par_setList[i] << std::endl;
            std::cout << "parallel Dijkstra (BST) gives " << result_par_BST[i] << std::endl;
            errors += 1;
        }
    }
    
    for (int i = 0; i < delta_stepp_par.n_vertices; i++)
    {
        if (delta_stepp.distance[i] != result_seq[i] || delta_stepp_par.distance[i] != result_seq[i])
        {
            std::cout << "error when computing shortest path to " << i << std::endl;
            std::cout << "Dijkstra gives " << result_seq[i] << std::endl;
            std::cout << "parallel delta stepping gives " << delta_stepp_par.distance[i] << std::endl;
            std::cout << "sequential delta stepping gives " << delta_stepp.distance[i] << std::endl;
            errors += 1;
        }
    }
    
    std::cout << errors << " errors" << std::endl;
    
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
    //compute_max_degree("test_graph.txt", true, true);
    //find_max("ca-HepPh_.txt", false);
    Graph *g1 = import_graph("test_graph.txt", true, true, false);
    //Graph *g1 = import_graph("5v_10e_d_w.txt", true, true, true);
    //Graph *g1 = import_graph("7e5v_5e5e_d_uw.txt", true, false, true);
    //Graph *g1 = import_graph("ca-HepPh_.txt", true, false, true);
    test(g1, delta, source, threads);
    
    return 0;
}
