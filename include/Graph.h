#ifndef GRAPH_H
#define GRAPH_H

#include <bits/stdc++.h>
#include "Node.h"

using namespace std;

class Graph {
private:
    vector<Node> nodes;

public:
    Graph(int numberOfNodes);

    void addEdge(int from, int to, int travelTime);
    void updateEdgeWeight(int from, int to, int travelTime);
    void closeRoad(int from, int to);
    void openRoad(int from, int to);
    void setNodePosition(int nodeId, double x, double y);

    
    const vector<Node>& getNodes() const;
};

#endif