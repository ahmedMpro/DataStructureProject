#pragma once
using namespace std;
#include <vector>  // include vector to store adjacency lists
#include <string>  // include string to pass status/error messages back to GUI code

// Graph class represents a graph that can be directed or undirected using an adjacency list
class Graph {
public:
    Graph(int vertexCount = 0, bool isDirected = false);  // constructor that records vertex count and edge direction mode

    bool configure(int vertexCount, bool isDirected);  // allow GUI to reconfigure vertex/direction without recreating object
    void clearEdges();  // drop all existing edges while keeping current configuration
    bool addEdge(int source, int destination);  // method to add an edge and report validation errors

    bool detectCycle() const;  // method to detect cycles using the appropriate strategy

    bool isDirected() const;  // expose configuration for GUI rendering
    int vertexCount() const;  // expose vertex count for GUI rendering
    const vector<vector<int>>& getAdjacencyList() const;  // allow GUI to inspect adjacency data
    const string& getLastError() const;  // expose last validation or processing error

private:
    bool detectCycleUndirected() const;  // helper dedicated to undirected cycle detection via Union-Find
    bool detectCycleDirected() const;  // helper dedicated to directed cycle detection via DFS recursion
    bool depthFirstDetectDirected(int vertex, vector<bool>& visited, vector<bool>& recursionStack) const;  // recursive DFS utility
    bool isValidVertex(int index) const;  // helper to verify vertex indices before use
    void setError(const string& message) const;  // record a human-readable error for GUI consumption
    void clearError() const;  // reset error indicator when operations succeed

    int vertexCount_;  // number of vertices in the graph
    bool isDirected_;  // flag indicating whether edges should be treated as directed or undirected
    vector<vector<int>> adjacencyList_;  // adjacency list representation
    mutable string lastError_;  // stores the most recent error so GUI can display it even from const methods
};
