#pragma once

#include "Graph.h"
#include "AStar.h"
#include <queue>
#include <vector>
#include <climits>
#include <utility>
#include <algorithm>
#include <cmath>

using namespace std;

class LPAStar {
private:
    const int INF = INT_MAX;

    vector<int> g;
    vector<int> rhs;
    vector<int> parent;

    vector<vector<pair<int, int>>> predecessorEdges;

    vector<int> queuedKey1;
    vector<int> queuedKey2;

    int source = -1;
    int destination = -1;
    int nodesExplored = 0;

    long long updateVertexCalls = 0;
    long long calculateKeyCalls = 0;
    long long predecessorChecks = 0;
    long long queuePushes = 0;
    long long queuePops = 0;
    long long staleEntries = 0;

    struct QueueNode {
        int node;
        int key1;
        int key2;

        bool operator>(const QueueNode& other) const {
            if (key1 != other.key1) {
                return key1 > other.key1;
            }

            if (key2 != other.key2) {
                return key2 > other.key2;
            }

            return node > other.node;
        }
    };

    priority_queue<
        QueueNode,
        vector<QueueNode>,
        greater<QueueNode>
    > open;

    pair<int, int> calculateKey(
        const Graph& graph,
        int node
    ) const;

public:
    void initialize(
        const Graph& graph,
        int source,
        int destination
    );

    void updateVertex(
        const Graph& graph,
        int node
    );

    void updateEdge(
        const Graph& graph,
        int from,
        int to
    );

    void computeShortestPath(
        const Graph& graph
    );

    RouteResult findShortestPath(
        const Graph& graph,
        int source,
        int destination
    );

    RouteResult getCurrentPath(
        const Graph& graph
    ) const;

    int getNodesExplored() const;

    void resetCounters();

    void printCounters() const;
};