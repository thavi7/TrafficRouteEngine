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
void Graph::updateEdgeWeight(int from, int to, int newTravelTime) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.travelTime = newTravelTime;
            break;
        }
    }

    for (auto& edge : nodes[to].edges) {
        if (edge.destination == from) {
            edge.travelTime = newTravelTime;
            break;
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
void Graph::increaseTraffic(int from, int to) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.travelTime += 5;
            break;
        }
    }

    for (auto& edge : nodes[to].edges) {
        if (edge.destination == from) {
            edge.travelTime += 5;
            break;
        }
    }
}

void Graph::decreaseTraffic(int from, int to) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.travelTime = max(edge.baseTravelTime, edge.travelTime - 5);
            break;
        }
    }

    for (auto& edge : nodes[to].edges) {
        if (edge.destination == from) {
            edge.travelTime = max(edge.baseTravelTime, edge.travelTime - 5);
            break;
        }
    }
}

void Graph::resetTraffic(int from, int to) {
    for (auto& edge : nodes[from].edges) {
        if (edge.destination == to) {
            edge.travelTime = edge.baseTravelTime;
            break;
        }
    }

    for (auto& edge : nodes[to].edges) {
        if (edge.destination == from) {
            edge.travelTime = edge.baseTravelTime;
            break;
        }
    }
}
    
const vector<Node>& Graph::getNodes() const {
    return nodes;
}