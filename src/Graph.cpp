#include "Graph.h"

Graph::Graph(int numberOfNodes) {
    for (int i = 0; i < numberOfNodes; i++) {
        nodes.emplace_back(i);
    }
}
// storing coordinates ok
void Graph::setNodePosition(int nodeId, double x, double y) {
    nodes[nodeId].x = x;
    nodes[nodeId].y = y;
}

//block is false initially for all yes
void Graph::addEdge(int from, int to, int travelTime) {
    nodes[from].edges.emplace_back(to, travelTime);
}
// from given node im traversing to every edges where our destination is matched we update that edge
void Graph::updateEdgeWeight(int from, int to, int travelTime) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.travelTime = travelTime;
            return;
        }
    }
}

void Graph::closeRoad(int from, int to) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.blocked = true;
            return;
        }
    }
}

void Graph::openRoad(int from, int to) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.blocked = false;
            return;
        }
    }
}

const vector<Node>& Graph::getNodes() const {
    return nodes;
}