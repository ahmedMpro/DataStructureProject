#include "graph.h"  // include the header declaring Graph and node structures

using namespace std;

Graph::Graph(int vertexCount)  // define constructor that takes number of vertices
    : vertexCount_(vertexCount),  // initialize member storing vertex count
      adjacencyList_(vertexCount)  // initialize adjacency list with empty vectors
{
    // constructor body intentionally empty because initialization list already set up members
}

void Graph::addEdge(int source, int destination)  // define method to add undirected edge
{
    adjacencyList_[source].push_back(destination);  // add destination to source neighbors
    adjacencyList_[destination].push_back(source);  // add source to destination neighbors (undirected)
}

bool Graph::detectCycle() const  // define cycle detection using Disjoint Set Union-Find
{
    DisjointSet set(vertexCount_);  // create Disjoint Set managing every vertex

    for (int vertex = 0; vertex < vertexCount_; ++vertex) {  // iterate over each vertex
        for (int neighbor : adjacencyList_[vertex]) {  // iterate over every adjacent vertex
            if (vertex < neighbor) {  // process each undirected edge only once
                int rootSource = set.find(vertex);  // find root of the current vertex
                int rootDestination = set.find(neighbor);  // find root of the neighboring vertex

                if (rootSource == rootDestination) {  // if both vertices share the same root
                    return true;  // cycle detected because adding edge creates a loop
                }

                set.unionSets(rootSource, rootDestination);  // otherwise, merge the two sets
            }
        }
    }

    return false;  // after processing all edges without finding a cycle, return false
}
