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
    
    void increaseTraffic(int from, int to);
    void decreaseTraffic(int from, int to);
    void resetTraffic(int from, int to);
    
    const vector<Node>& getNodes() const;
};

#endif