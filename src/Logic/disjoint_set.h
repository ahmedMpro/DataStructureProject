#pragma once

#include <QVector>  // use Qt container to avoid STL

// DisjointSet class supports union-find operations with path compression and union by rank
class DisjointSet {
public:
    explicit DisjointSet(int size);  // constructor that initializes tables with given size

    int find(int node);  // find operation with path compression

    void unionSets(int firstRoot, int secondRoot);  // union by rank operation

private:
    QVector<int> parent_;  // parent array storing representative for each node
    QVector<int> rank_;  // rank array guiding union to keep trees shallow
};
