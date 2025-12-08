#include "Logic/graph.h"  // include the header declaring Graph interfaces
#include "Logic/disjoint_set.h"  // include the Disjoint Set implementation needed only in this translation unit

using namespace std;  // remain within the implementation file to keep headers clean

Graph::Graph(int vertexCount, bool isDirected)  // define constructor that takes number of vertices and direction flag
    : vertexCount_(0),  // default to zero until configuration is validated
      isDirected_(isDirected),  // store whether the caller wants a directed or undirected graph
      adjacencyList_(),  // will be sized by configure
      lastError_()  // start with no error
{
    configure(vertexCount, isDirected);  // reuse configuration logic to initialize storage safely
}

bool Graph::configure(int vertexCount, bool isDirected)  // allow GUI to reconfigure the graph on demand
{
    if (vertexCount < 0) {  // reject negative vertex counts because vectors cannot have negative sizes
        setError("Vertex count cannot be negative.");  // store explanation for GUI display
        vertexCount_ = 0;  // reset to a safe value
        adjacencyList_.clear();  // ensure adjacency list matches the safe count
        return false;  // signal configuration failure
    }

    vertexCount_ = vertexCount;  // store the validated vertex count
    isDirected_ = isDirected;  // store updated direction flag
    adjacencyList_.assign(vertexCount_, vector<int>());  // resize adjacency list to match the new vertex count
    clearError();  // configuration succeeded, clear any previous error
    return true;  // report success to the caller (GUI)
}

void Graph::clearEdges()  // drop all edges while retaining the current vertex configuration
{
    for (vector<int>& neighbors : adjacencyList_) {  // iterate over every adjacency bucket
        neighbors.clear();  // remove stored neighbors while keeping capacity for reuse
    }

    clearError();  // clearing edges is always safe, so reset error state
}

bool Graph::addEdge(int source, int destination)  // define method to add an edge whose behavior depends on the graph type
{
    if (!isValidVertex(source) || !isValidVertex(destination)) {  // verify both endpoints fall inside the adjacency list bounds
        setError("Ignored edge with out-of-range endpoint(s).");  // record issue so GUI can display it
        return false;  // stop processing because the edge would corrupt memory
    }

    adjacencyList_[source].push_back(destination);  // always add the forward edge from source to destination

    if (!isDirected_) {  // only add the reverse connection when the graph is undirected
        adjacencyList_[destination].push_back(source);  // add source to destination neighbors (undirected)
    }

    clearError();  // edge insertion succeeded, clear previous errors
    return true;  // return success so GUI buttons can react accordingly
}

bool Graph::detectCycle() const  // dispatch cycle detection based on whether the graph is directed
{
    if (vertexCount_ <= 0) {  // empty graphs cannot contain cycles, but ensure error state is reset
        clearError();  // no issue detected
        return false;  // nothing to process
    }

    if (isDirected_) {  // for directed graphs, use DFS-based detection
        return detectCycleDirected();  // call helper specialized for directed cycles
    }

    return detectCycleUndirected();  // otherwise fall back to the Union-Find strategy for undirected graphs
}

bool Graph::detectCycleUndirected() const  // define cycle detection using Disjoint Set Union-Find
{
    DisjointSet set(vertexCount_);  // create Disjoint Set managing every vertex

    for (int vertex = 0; vertex < vertexCount_; ++vertex) {  // iterate over each vertex
        for (int neighbor : adjacencyList_[vertex]) {  // iterate over every adjacent vertex
            if (vertex == neighbor) {  // immediately treat self-loops as cycles because they form a closed path of length one
                return true;  // report cycle without further processing
            }

            if (vertex < neighbor) {  // process each undirected edge only once by enforcing an ordering
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

bool Graph::detectCycleDirected() const  // define directed cycle detection via depth-first search with recursion stack
{
    vector<bool> visited(vertexCount_, false);  // track which vertices have been fully visited
    vector<bool> recursionStack(vertexCount_, false);  // track the recursion stack to catch back edges

    for (int vertex = 0; vertex < vertexCount_; ++vertex) {  // iterate through all vertices to cover disconnected components
        if (!visited[vertex]) {  // start a DFS only if the vertex has not been visited yet
            if (depthFirstDetectDirected(vertex, visited, recursionStack)) {  // run recursive helper and check if it reports a cycle
                return true;  // propagate the discovery immediately
            }
        }
    }

    return false;  // if every DFS finished without finding a cycle, report false
}

bool Graph::depthFirstDetectDirected(int vertex, vector<bool>& visited, vector<bool>& recursionStack) const  // recursive DFS helper
{
    visited[vertex] = true;  // mark the current vertex as visited to avoid reprocessing it within other branches
    recursionStack[vertex] = true;  // mark the vertex as currently in the recursion stack

    for (int neighbor : adjacencyList_[vertex]) {  // inspect each outgoing edge from the current vertex
        if (vertex == neighbor) {  // if the edge points to the same vertex, it forms a self-loop cycle
            return true;  // immediately signal cycle presence
        }

        if (!visited[neighbor]) {  // proceed only if the neighbor has not been visited yet
            if (depthFirstDetectDirected(neighbor, visited, recursionStack)) {  // recursively explore deeper paths
                return true;  // bubble up the positive result if a cycle is found deeper in the recursion
            }
        } else if (recursionStack[neighbor]) {  // if the neighbor is in the current recursion stack, a back edge exists
            return true;  // detect the cycle formed by the back edge
        }
    }

    recursionStack[vertex] = false;  // unmark the vertex when backtracking out of the recursion stack
    return false;  // no cycle found along this path
}

bool Graph::isValidVertex(int index) const  // helper definition kept in the implementation file
{
    return index >= 0 && index < vertexCount_;  // vertex is valid when it falls inside the inclusive-exclusive range [0, vertexCount_)
}

bool Graph::isDirected() const  // expose whether edges are interpreted as directed
{
    return isDirected_;
}

int Graph::vertexCount() const  // expose number of vertices for GUI controls
{
    return vertexCount_;
}

const vector<vector<int>>& Graph::getAdjacencyList() const  // provide read-only access to adjacency data
{
    return adjacencyList_;
}

const string& Graph::getLastError() const  // expose last error for GUI labels
{
    return lastError_;
}

void Graph::setError(const string& message) const  // helper for recording last error message
{
    lastError_ = message;
}

void Graph::clearError() const  // reset last error to an empty string
{
    lastError_.clear();
}
