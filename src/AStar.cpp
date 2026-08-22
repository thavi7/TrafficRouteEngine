#include "AStar.h"
#include <bits/stdc++.h>

using namespace std;

const int INF = INT_MAX;

double AStar::heuristic(const Node& currentNode, const Node& destinationNode) {
    // double dx = currentNode.x - destinationNode.x;
    // double dy = currentNode.y - destinationNode.y;
    // return sqrt(dx * dx + dy * dy);
    
    return fabs(currentNode.x - destinationNode.x) + fabs(currentNode.y - destinationNode.y);
}

RouteResult AStar::findShortestPath(const Graph& graph, int source, int destination) {
    const vector<Node>& nodes = graph.getNodes();

    int n = nodes.size();

    vector<int> dist(n, INF);
    vector<int> parent(n, -1);

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    int nodesExplored = 0;

    dist[source] = 0;
    pq.push({heuristic(nodes[source], nodes[destination]), source});

    while (!pq.empty()) {
        int currentNode = pq.top().second;
        pq.pop();

        nodesExplored++;

        if (currentNode == destination) {
            break;
        }

        for (const auto& edge : nodes[currentNode].edges) {
            if (edge.blocked) {
                continue;
            }

            int newDistance = dist[currentNode] + edge.travelTime;

            if (newDistance < dist[edge.destination]) {
                dist[edge.destination] = newDistance;
                parent[edge.destination] = currentNode;

                double priority = newDistance + heuristic(
                    nodes[edge.destination],
                    nodes[destination]
                );

                pq.push({priority, edge.destination});
            }
        }
    }

    if (dist[destination] == INF) {
        return {{}, -1, nodesExplored};
    }

    vector<int> path;

    int current = destination;

    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }

    reverse(path.begin(), path.end());

    return {path, dist[destination], nodesExplored};
}