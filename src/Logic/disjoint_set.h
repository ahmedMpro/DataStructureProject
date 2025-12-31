#pragma once

#include <QVector>  


class DisjointSet {
public:
    explicit DisjointSet(int size);  

    int find(int node);  

    void unionSets(int firstRoot, int secondRoot);  

private:
    QVector<int> parent_;  
    QVector<int> rank_;  
};
