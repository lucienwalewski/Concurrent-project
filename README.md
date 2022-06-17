# Parallel SSCP

This project contains the implementation of sequential and parallel single-source shortest paths algorithms.
The implemented algorithms are : 
- Dijkstra
- Parallel Dijkstra
- Delta Stepping
- Parallel Delta stepping 

In order to test our code, go into the ```Graphs``` folder in your terminal and run the following command lines.

## Compilation

```bash
g++ -std=c++17 -O3 -o main *.cpp
```


## run 
```
./main source number_of_threads delta 
```

- ```source``` corresponds to the number of the source vertex from whoch we will run the shortest paths algorithms
- ```delta``` is the value one wants to use for the two Delta Stepping algorithms

## example 
```
./main 0 2 20
```
In the above example, we start the shortest path algorithms from the vertex labeled by ```0``` using two threads for the parallel algorithms and the value of ```delta``` is set to twenty.

## Authors
- Alban Puech
- Luca Bonengel
- Lucien Walewski
