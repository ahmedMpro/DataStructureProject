#pragma once
#include "disjoint_set.h"  // include the Disjoint Set implementation used for cycle detection

#include <vector>          // include vector to store adjacency lists

using namespace std;

// Graph class represents an undirected graph with an adjacency list
class Graph {
public:
    Graph(int vertexCount);  // constructor declaration

    void addEdge(int source, int destination);  // method to add undirected edge

    bool detectCycle() const;  // method to detect cycles using Disjoint Set structure

private:
    int vertexCount_;  // number of vertices in the graph
    std::vector<std::vector<int>> adjacencyList_;  // adjacency list representation
};
