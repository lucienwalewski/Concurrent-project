# Parallel SSCP

This project contains the implementation of sequential and parallel single-source shortest paths algorithms.
The implemented algorithms are : 
- Dijkstra
- Parallel Dijkstra
- Delta Stepping
- Parallel Delta stepping 

In order to test our code, go into the Graphs folder in your terminal and run the following command lines.

## Compilation

```bash
g++ -std=c++17 -O3 -o main *.cpp
```


## run 
```
./main source number_of_threads delta 
```
## example 
```
./main 0 2 20
```


## Authors
- Alban Puech
- Luca Bonengel
- Lucien Walewski
