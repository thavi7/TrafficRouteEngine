#ifndef ASTAR_H
#define ASTAR_H

#include "Graph.h"
#include "RouteResult.h"

using namespace std;

class AStar {
private:
    double heuristic(const Node& currentNode, const Node& destinationNode);

public:
    RouteResult findShortestPath(const Graph& graph, int source, int destination);
};

#endif