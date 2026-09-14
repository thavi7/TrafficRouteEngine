#include "LPAStar.h"

pair<int, int> LPAStar::calculateKey(
    const Graph& graph,
    int node
) const {
    int minimum = min(g[node], rhs[node]);

    if (minimum == INF) {
        return {
            INF,
            INF
        };
    }

    const vector<Node>& nodes =
        graph.getNodes();

    int dx =
        abs(
            nodes[node].x -
            nodes[destination].x
        );

    int dy =
        abs(
            nodes[node].y -
            nodes[destination].y
        );

    int heuristic =
        dx + dy;

    return {
        minimum + heuristic,
        minimum
    };
}

vector<int> LPAStar::getPredecessors(
    const Graph& graph,
    int node
) const {
    const vector<Node>& nodes =
        graph.getNodes();

    vector<int> predecessors;

    for (int i = 0;
         i < static_cast<int>(nodes.size());
         i++) {

        for (const auto& edge :
             nodes[i].edges) {

            if (edge.destination == node) {
                predecessors.push_back(i);
            }
        }
    }

    return predecessors;
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

    nodesExplored = 0;

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
}

void LPAStar::updateVertex(
    const Graph& graph,
    int node
) {
    if (node != source) {
        rhs[node] = INF;
        parent[node] = -1;

        const vector<Node>& nodes =
            graph.getNodes();

        vector<int> predecessors =
            getPredecessors(
                graph,
                node
            );

        for (int predecessor :
             predecessors) {

            for (const auto& edge :
                 nodes[predecessor].edges) {

                if (edge.destination != node) {
                    continue;
                }

                if (edge.blocked) {
                    continue;
                }

                if (g[predecessor] == INF) {
                    continue;
                }

                int candidate =
                    g[predecessor] +
                    edge.travelTime;

                if (candidate < rhs[node]) {
                    rhs[node] = candidate;
                    parent[node] = predecessor;
                }
            }
        }
    }

    if (g[node] != rhs[node]) {
        pair<int, int> key =
            calculateKey(
                graph,
                node
            );

        open.push({
            node,
            key.first,
            key.second
        });
    }
}

void LPAStar::updateEdge(
    const Graph& graph,
    int from,
    int to
) {
    updateVertex(
        graph,
        to
    );

    updateVertex(
        graph,
        from
    );

    const vector<Node>& nodes =
        graph.getNodes();

    for (const auto& edge :
         nodes[from].edges) {

        updateVertex(
            graph,
            edge.destination
        );
    }

    for (const auto& edge :
         nodes[to].edges) {

        updateVertex(
            graph,
            edge.destination
        );
    }
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

        pair<int, int> currentKey =
            calculateKey(
                graph,
                current.node
            );

        if (current.key1 != currentKey.first ||
            current.key2 != currentKey.second) {

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

            const vector<Node>& nodes =
                graph.getNodes();

            for (const auto& edge :
                 nodes[current.node].edges) {

                updateVertex(
                    graph,
                    edge.destination
                );
            }

        } else {

            g[current.node] =
                INF;

            updateVertex(
                graph,
                current.node
            );

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