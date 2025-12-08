#pragma once

#include <vector>  // include vector to hold parent and rank arrays

// DisjointSet class supports union-find operations with path compression and union by rank
class DisjointSet {
public:
    explicit DisjointSet(int size);  // constructor that initializes tables with given size

    int find(int node);  // find operation with path compression

    void unionSets(int firstRoot, int secondRoot);  // union by rank operation

private:
    std::vector<int> parent_;  // parent array storing representative for each node
    std::vector<int> rank_;  // rank array guiding union to keep trees shallow
};
