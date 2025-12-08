#include "Logic/graph.h"

#include <iostream>
#include <string>

int main_example() {
    Graph cyclicGraph(3);
    cyclicGraph.addEdge(0, 1);
    cyclicGraph.addEdge(1, 2);
    cyclicGraph.addEdge(2, 0);

    bool hasCycleInCyclic = cyclicGraph.detectCycle();
    std::string cyclicMessage = hasCycleInCyclic ? "undirected graph with cycle" : "undirected graph without cycle";
    std::cout << "Graph 1: " << cyclicMessage << '\n';

    Graph acyclicGraph(3);
    acyclicGraph.addEdge(0, 1);
    acyclicGraph.addEdge(1, 2);

    bool hasCycleInAcyclic = acyclicGraph.detectCycle();
    std::string acyclicMessage = hasCycleInAcyclic ? "unexpected cycle in undirected graph" : "undirected graph remains acyclic";
    std::cout << "Graph 2: " << acyclicMessage << '\n';

    Graph directedCyclic(3, true);
    directedCyclic.addEdge(0, 1);
    directedCyclic.addEdge(1, 2);
    directedCyclic.addEdge(2, 0);

    bool hasDirectedCycle = directedCyclic.detectCycle();
    std::string directedCycleMessage = hasDirectedCycle ? "directed graph with cycle" : "directed graph without cycle";
    std::cout << "Graph 3: " << directedCycleMessage << '\n';

    Graph directedAcyclic(4, true);
    directedAcyclic.addEdge(0, 1);
    directedAcyclic.addEdge(1, 2);
    directedAcyclic.addEdge(2, 3);

    bool hasDirectedAcyclicCycle = directedAcyclic.detectCycle();
    std::string directedAcyclicMessage = hasDirectedAcyclicCycle ? "directed acyclic graph reported a cycle" : "directed acyclic graph confirmed";
    std::cout << "Graph 4: " << directedAcyclicMessage << '\n';

    return 0;
}
