#pragma once
using namespace std;
#include <QVector>
#include <QString>

class Graph {
public:
    struct AdjNode {
        int dest;
        AdjNode* next;
    };

    Graph(int vertexCount = 0, bool isDirected = false);
    ~Graph();

    bool configure(int vertexCount, bool isDirected);
    void clearEdges();
    bool addEdge(int source, int destination);
    bool removeEdge(int source, int destination);

    bool detectCycle() const;

    bool isDirected() const;
    int vertexCount() const;
    QVector<QVector<int>> getAdjacencyList() const;
    const QString& getLastError() const;

private:
    bool detectCycleUndirected() const;
    bool detectCycleDirected() const;
    bool depthFirstDetectDirected(int vertex, QVector<bool>& visited, QVector<bool>& recursionStack) const;
    bool isValidVertex(int index) const;
    void setError(const QString& message) const;
    void clearError() const;

    void clearAdjacency();
    void appendNeighbor(int source, int destination);

    int vertexCount_;
    bool isDirected_;
    AdjNode** adjacency_{nullptr};
    mutable QString lastError_;
};
