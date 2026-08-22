#include <iostream>
#include "GraphGenerator.h"
#include "Dijkstra.h"
#include "AStar.h"

using namespace std;

void printRoute(const string& name, const RouteResult& result) {
    cout << name << ":\n";
    cout << "Total travel time: " << result.totalTime << "\n";
    cout << "Nodes explored: " << result.nodesExplored << "\n\n";
}

int main() {
    Graph graph = GraphGenerator::generateGrid(100, 100);

    int source = 1000;
    int destination = 9999;

    Dijkstra dijkstra;
    AStar aStar;

    RouteResult dijkstraResult = dijkstra.findShortestPath(graph, source, destination);
    RouteResult aStarResult = aStar.findShortestPath(graph, source, destination);

    printRoute("Dijkstra", dijkstraResult);
    printRoute("A*", aStarResult);

    return 0;
}