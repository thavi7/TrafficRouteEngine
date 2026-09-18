#include "LPAStar.h"
#include <iostream>

pair<int, int> LPAStar::calculateKey(
    const Graph& graph,
    int node
) const {
    const_cast<LPAStar*>(this)->calculateKeyCalls++;

    int minimum = min(g[node], rhs[node]);

    if (minimum == INF) {
        return {
            INF,
            INF
        };
    }

    const vector<Node>& nodes =
        graph.getNodes();

    int dx = abs(
        nodes[node].x -
        nodes[destination].x
    );

    int dy = abs(
        nodes[node].y -
        nodes[destination].y
    );

    int heuristic = dx + dy;

    return {
        minimum + heuristic,
        minimum
    };
}

void LPAStar::resetCounters() {
    updateVertexCalls = 0;
    calculateKeyCalls = 0;
    predecessorChecks = 0;
    queuePushes = 0;
    queuePops = 0;
    staleEntries = 0;
}

void LPAStar::initialize(
    const Graph& graph,
    int source,
    int destination
) {
    int n =
        static_cast<int>(
            graph.getNodes().size()
        );

    this->source = source;
    this->destination = destination;

    g.assign(n, INF);
    rhs.assign(n, INF);
    parent.assign(n, -1);

    predecessorEdges.assign(n, {});

    const vector<Node>& nodes =
        graph.getNodes();

    for (int from = 0; from < n; from++) {
        for (int edgeIndex = 0;
             edgeIndex < static_cast<int>(nodes[from].edges.size());
             edgeIndex++) {

            int to =
                nodes[from].edges[edgeIndex].destination;

            predecessorEdges[to].push_back({
                from,
                edgeIndex
            });
        }
    }

    queuedKey1.assign(n, INF);
    queuedKey2.assign(n, INF);

    nodesExplored = 0;

    resetCounters();

    while (!open.empty()) {
        open.pop();
    }

    rhs[source] = 0;

    pair<int, int> key =
        calculateKey(
            graph,
            source
        );

    open.push({
        source,
        key.first,
        key.second
    });

    queuePushes++;

    queuedKey1[source] = key.first;
    queuedKey2[source] = key.second;
}

void LPAStar::updateVertex(
    const Graph& graph,
    int node
) {
    updateVertexCalls++;

    if (node != source) {
        int newRhs = INF;
        int newParent = -1;

        const vector<Node>& nodes =
            graph.getNodes();

        for (const auto& predecessor :
             predecessorEdges[node]) {

            predecessorChecks++;

            int predecessorNode =
                predecessor.first;

            int edgeIndex =
                predecessor.second;

            const Edge& edge =
                nodes[predecessorNode].edges[edgeIndex];

            if (edge.blocked) {
                continue;
            }

            if (g[predecessorNode] == INF) {
                continue;
            }

            int candidate =
                g[predecessorNode] +
                edge.travelTime;

            if (candidate < newRhs) {
                newRhs = candidate;
                newParent = predecessorNode;
            }
        }

        rhs[node] = newRhs;
        parent[node] = newParent;
    }

    if (g[node] != rhs[node]) {
        pair<int, int> key =
            calculateKey(
                graph,
                node
            );

        if (queuedKey1[node] != key.first ||
            queuedKey2[node] != key.second) {

            open.push({
                node,
                key.first,
                key.second
            });

            queuePushes++;

            queuedKey1[node] = key.first;
            queuedKey2[node] = key.second;
        }
    } else {
        queuedKey1[node] = INF;
        queuedKey2[node] = INF;
    }
}

void LPAStar::updateEdge(
    const Graph& graph,
    int from,
    int to
) {
    updateVertex(
        graph,
        from
    );

    updateVertex(
        graph,
        to
    );
}

void LPAStar::computeShortestPath(
    const Graph& graph
) {
    while (!open.empty()) {
        QueueNode current =
            open.top();

        pair<int, int> goalKey =
            calculateKey(
                graph,
                destination
            );

        if (current.key1 > goalKey.first ||
            (current.key1 == goalKey.first &&
             current.key2 >= goalKey.second)) {

            if (g[destination] ==
                rhs[destination]) {

                break;
            }
        }

        open.pop();
        queuePops++;

        if (queuedKey1[current.node] != current.key1 ||
            queuedKey2[current.node] != current.key2) {

            staleEntries++;

            continue;
        }

        queuedKey1[current.node] = INF;
        queuedKey2[current.node] = INF;

        pair<int, int> currentKey =
            calculateKey(
                graph,
                current.node
            );

        if (current.key1 != currentKey.first ||
            current.key2 != currentKey.second) {

            staleEntries++;

            continue;
        }

        if (g[current.node] ==
            rhs[current.node]) {

            continue;
        }

        nodesExplored++;

        if (g[current.node] >
            rhs[current.node]) {

            g[current.node] =
                rhs[current.node];

        } else {

            g[current.node] =
                INF;

            updateVertex(
                graph,
                current.node
            );
        }

        const vector<Node>& nodes =
            graph.getNodes();

        for (const auto& edge :
             nodes[current.node].edges) {

            updateVertex(
                graph,
                edge.destination
            );
        }
    }
}

RouteResult LPAStar::findShortestPath(
    const Graph& graph,
    int source,
    int destination
) {
    initialize(
        graph,
        source,
        destination
    );

    computeShortestPath(
        graph
    );

    return getCurrentPath(
        graph
    );
}

RouteResult LPAStar::getCurrentPath(
    const Graph& graph
) const {
    if (destination < 0 ||
        destination >= static_cast<int>(g.size()) ||
        g[destination] == INF) {

        return {
            {},
            -1,
            nodesExplored
        };
    }

    vector<int> path;

    int current =
        destination;

    while (current != -1) {
        path.push_back(current);

        if (current == source) {
            break;
        }

        current =
            parent[current];
    }

    if (path.empty() ||
        path.back() != source) {

        return {
            {},
            -1,
            nodesExplored
        };
    }

    reverse(
        path.begin(),
        path.end()
    );

    return {
        path,
        g[destination],
        nodesExplored
    };
}

int LPAStar::getNodesExplored() const {
    return nodesExplored;
}

void LPAStar::printCounters() const {
    cout << "\n========== LPA* INTERNAL COUNTERS ==========\n";
    cout << "updateVertex calls: " << updateVertexCalls << "\n";
    cout << "calculateKey calls: " << calculateKeyCalls << "\n";
    cout << "predecessor checks: " << predecessorChecks << "\n";
    cout << "queue pushes: " << queuePushes << "\n";
    cout << "queue pops: " << queuePops << "\n";
    cout << "stale entries: " << staleEntries << "\n";
    cout << "nodes explored: " << nodesExplored << "\n";
    cout << "============================================\n";
}