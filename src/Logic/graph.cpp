#include "Logic/graph.h"
#include "Logic/disjoint_set.h"

namespace {
    static void collectNeighbors(const Graph::AdjNode* head, QVector<int>& out) {
        const Graph::AdjNode* current = head;
        while (current) {
            out.push_back(current->dest);
            current = current->next;
        }
    }
}

Graph::Graph(int vertexCount, bool isDirected)
    : vertexCount_(0), isDirected_(isDirected), adjacency_(nullptr), lastError_() {
    configure(vertexCount, isDirected);
}

Graph::~Graph() {
    clearAdjacency();
}

bool Graph::configure(int vertexCount, bool isDirected) {
    if (vertexCount < 0) {
        setError("Vertex count cannot be negative.");
        clearAdjacency();
        vertexCount_ = 0;
        adjacency_ = nullptr;
        return false;
    }

    clearAdjacency();
    vertexCount_ = vertexCount;
    isDirected_ = isDirected;

    if (vertexCount_ > 0) {
        adjacency_ = new AdjNode*[vertexCount_];
        for (int i = 0; i < vertexCount_; ++i) {
            adjacency_[i] = nullptr;
        }
    } else {
        adjacency_ = nullptr;
    }

    clearError();
    return true;
}

void Graph::clearAdjacency() {
    if (!adjacency_) {
        return;
    }

    for (int i = 0; i < vertexCount_; ++i) {
        AdjNode* current = adjacency_[i];
        while (current) {
            AdjNode* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        adjacency_[i] = nullptr;
    }
    delete[] adjacency_;
    adjacency_ = nullptr;
}

void Graph::clearEdges() {
    clearAdjacency();
    if (vertexCount_ > 0) {
        adjacency_ = new AdjNode*[vertexCount_];
        for (int i = 0; i < vertexCount_; ++i) {
            adjacency_[i] = nullptr;
        }
    }
    clearError();
}

void Graph::appendNeighbor(int source, int destination) {
    AdjNode* node = new AdjNode{destination, adjacency_[source]};
    adjacency_[source] = node;
}

bool Graph::addEdge(int source, int destination) {
    if (!isValidVertex(source) || !isValidVertex(destination)) {
        setError("Ignored edge with out-of-range endpoint(s).");
        return false;
    }

    appendNeighbor(source, destination);
    if (!isDirected_) {
        appendNeighbor(destination, source);
    }

    clearError();
    return true;
}

bool Graph::removeEdge(int source, int destination) {
    if (!isValidVertex(source) || !isValidVertex(destination)) {
        setError("Ignored removal with out-of-range endpoint(s).");
        return false;
    }

    auto removeNeighbor = [this](int from, int to) {
        AdjNode* prev = nullptr;
        AdjNode* current = adjacency_[from];
        while (current) {
            if (current->dest == to) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    adjacency_[from] = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    };

    bool removed = removeNeighbor(source, destination);
    if (!isDirected_) {
        removeNeighbor(destination, source);
    }

    if (removed) {
        clearError();
        return true;
    }

    setError("Ignored removal for non-existent edge.");
    return false;
}

bool Graph::detectCycle() const {
    if (vertexCount_ <= 0) {
        clearError();
        return false;
    }

    if (isDirected_) {
        return detectCycleDirected();
    }

    return detectCycleUndirected();
}

bool Graph::detectCycleUndirected() const {
    DisjointSet set(vertexCount_);

    for (int vertex = 0; vertex < vertexCount_; ++vertex) {
        const AdjNode* current = adjacency_ ? adjacency_[vertex] : nullptr;
        while (current) {
            const int neighbor = current->dest;
            if (vertex == neighbor) {
                return true;  // self-loop
            }

            if (vertex < neighbor) {  // process each undirected edge once
                int rootSource = set.find(vertex);
                int rootDestination = set.find(neighbor);

                if (rootSource == rootDestination) {
                    return true;
                }

                set.unionSets(rootSource, rootDestination);
            }
            current = current->next;
        }
    }

    return false;
}

bool Graph::detectCycleDirected() const {
    QVector<bool> visited(vertexCount_, false);
    QVector<bool> recursionStack(vertexCount_, false);

    for (int vertex = 0; vertex < vertexCount_; ++vertex) {
        if (!visited[vertex]) {
            if (depthFirstDetectDirected(vertex, visited, recursionStack)) {
                return true;
            }
        }
    }

    return false;
}

bool Graph::depthFirstDetectDirected(int vertex, QVector<bool>& visited, QVector<bool>& recursionStack) const {
    visited[vertex] = true;
    recursionStack[vertex] = true;

    const AdjNode* current = adjacency_ ? adjacency_[vertex] : nullptr;
    while (current) {
        const int neighbor = current->dest;
        if (vertex == neighbor) {
            return true;  // self-loop
        }

        if (!visited[neighbor]) {
            if (depthFirstDetectDirected(neighbor, visited, recursionStack)) {
                return true;
            }
        } else if (recursionStack[neighbor]) {
            return true;
        }
        current = current->next;
    }

    recursionStack[vertex] = false;
    return false;
}

bool Graph::isValidVertex(int index) const {
    return index >= 0 && index < vertexCount_;
}

bool Graph::isDirected() const {
    return isDirected_;
}

int Graph::vertexCount() const {
    return vertexCount_;
}

QVector<QVector<int>> Graph::getAdjacencyList() const {
    QVector<QVector<int>> view(vertexCount_);
    if (!adjacency_) {
        return view;
    }
    for (int vertex = 0; vertex < vertexCount_; ++vertex) {
        collectNeighbors(adjacency_[vertex], view[vertex]);
    }
    return view;
}

const QString& Graph::getLastError() const {
    return lastError_;
}

void Graph::setError(const QString& message) const {
    lastError_ = message;
}

void Graph::clearError() const {
    lastError_.clear();
}