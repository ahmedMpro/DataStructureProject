#include "graph.h"  // include the graph class and its cycle detection helper

#include <iostream>  // include iostream for console output
#include <string>    // include string to hold descriptive messages

using namespace std;

int main()  // program entry point returning an integer status code
{
    Graph cyclicGraph(3);  // create a graph instance configured with three vertices
    cyclicGraph.addEdge(0, 1);  // connect vertex 0 with vertex 1
    cyclicGraph.addEdge(1, 2);  // connect vertex 1 with vertex 2
    cyclicGraph.addEdge(2, 0);  // connect vertex 2 back to vertex 0, creating a cycle

    bool hasCycleInCyclic = cyclicGraph.detectCycle();  // check if the first graph contains a cycle
    string cyclicMessage = hasCycleInCyclic ? "cyclic graph contains a cycle" : "cyclic graph is acyclic";  // prepare message for the cyclic graph
    cout << "Graph 1: " << cyclicMessage << '\n';  // print the result for the first graph

    Graph acyclicGraph(3);  // create a second graph instance, also with three vertices
    acyclicGraph.addEdge(0, 1);  // connect vertex 0 with vertex 1
    acyclicGraph.addEdge(1, 2);  // connect vertex 1 with vertex 2 only, no closing edge

    bool hasCycleInAcyclic = acyclicGraph.detectCycle();  // check if the second graph contains a cycle
    string acyclicMessage = hasCycleInAcyclic ? "acyclic graph contains a cycle" : "acyclic graph is acyclic";  // prepare message for the second graph
    cout << "Graph 2: " << acyclicMessage << '\n';  // print the result for the second graph

    return 0;  // return zero to indicate successful execution
}
