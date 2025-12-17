#pragma once
using namespace std;
#include <string>   // include string to pass status/error messages back to GUI code
#include <vector>   // still used for returned views and helpers

// Graph class represents a graph that can be directed or undirected using an adjacency list
class Graph {
public:
    struct AdjNode {
        int dest;
        AdjNode* next;
    };

    Graph(int vertexCount = 0, bool isDirected = false);  // constructor that records vertex count and edge direction mode
    ~Graph();  // destructor to free adjacency lists

    bool configure(int vertexCount, bool isDirected);  // allow GUI to reconfigure vertex/direction without recreating object
    void clearEdges();  // drop all existing edges while keeping current configuration
    bool addEdge(int source, int destination);  // method to add an edge and report validation errors
    bool removeEdge(int source, int destination);  // allow the UI to remove an existing edge

    bool detectCycle() const;  // method to detect cycles using the appropriate strategy

    bool isDirected() const;  // expose configuration for GUI rendering
    int vertexCount() const;  // expose vertex count for GUI rendering
    vector<vector<int>> getAdjacencyList() const;  // allow GUI to inspect adjacency data
    const string& getLastError() const;  // expose last validation or processing error

private:
    bool detectCycleUndirected() const;  // helper dedicated to undirected cycle detection via Union-Find
    bool detectCycleDirected() const;  // helper dedicated to directed cycle detection via DFS recursion
    bool depthFirstDetectDirected(int vertex, vector<bool>& visited, vector<bool>& recursionStack) const;  // recursive DFS utility
    bool isValidVertex(int index) const;  // helper to verify vertex indices before use
    void setError(const string& message) const;  // record a human-readable error for GUI consumption
    void clearError() const;  // reset error indicator when operations succeed

    void clearAdjacency();  // free all adjacency nodes
    void appendNeighbor(int source, int destination);  // add neighbor to adjacency list

    int vertexCount_;  // number of vertices in the graph
    bool isDirected_;  // flag indicating whether edges should be treated as directed or undirected
    AdjNode** adjacency_{nullptr};  // array of adjacency list heads (linked lists)
    mutable string lastError_;  // stores the most recent error so GUI can display it even from const methods
};
