#include "Dijkstra.h"
#include <bits/stdc++.h>

const int INF=INT_MAX;

RouteResult Dijkstra::findShortestPath(const Graph& graph, int source, int destination) {
    const vector<Node>& nodes = graph.getNodes();

    int n = nodes.size();

    vector<int> dist(n, INF);
    vector<int> parent(n, -1);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int nodesExplored = 0;
    dist[source] = 0;
    pq.push({0, source});
//staring dijstra...
    while (!pq.empty()) {
        int currentDistance = pq.top().first;
        int currentNode = pq.top().second;
        pq.pop();

        if (currentDistance != dist[currentNode]) {
            continue;
        }
        nodesExplored++;
        if (currentNode == destination) {
            break;
        }

        for (const auto& edge : nodes[currentNode].edges) {
            if (edge.blocked) {
                continue;
            }

            int newDistance = currentDistance + edge.travelTime;

            if (newDistance < dist[edge.destination]) {
                dist[edge.destination] = newDistance;
                parent[edge.destination] = currentNode;

                pq.push({newDistance, edge.destination});
            }
        }
    }

    if (dist[destination] == INF) {
        return {{}, -1,nodesExplored};
    }

    vector<int> path;

    int current = destination;

    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }

    reverse(path.begin(), path.end());

    return {path, dist[destination],nodesExplored};
}
