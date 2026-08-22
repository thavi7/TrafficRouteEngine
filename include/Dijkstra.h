#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <bits/stdc++.h>
#include "Graph.h"
#include "RouteResult.h"

using namespace std;


class Dijkstra {
public:
    RouteResult findShortestPath(const Graph& graph, int source, int destination);
};

#endif