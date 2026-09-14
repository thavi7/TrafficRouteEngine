#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

#include "GraphGenerator.h"
#include "AStar.h"
#include "LPAStar.h"

using namespace std;
using namespace chrono;

bool isRouteEdge(
    const vector<int>& path,
    int from,
    int to
) {
    for (int i = 0; i + 1 < static_cast<int>(path.size()); i++) {
        if ((path[i] == from && path[i + 1] == to) ||
            (path[i] == to && path[i + 1] == from)) {
            return true;
        }
    }

    return false;
}

vector<pair<int, int>> getAllEdges(
    const Graph& graph
) {
    vector<pair<int, int>> edges;

    const vector<Node>& nodes =
        graph.getNodes();

    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        for (const auto& edge : nodes[i].edges) {
            if (i < edge.destination) {
                edges.push_back({
                    i,
                    edge.destination
                });
            }
        }
    }

    return edges;
}

pair<int, int> findOnRouteEdge(
    const vector<int>& path
) {
    if (path.size() < 2) {
        return {-1, -1};
    }

    int middle =
        static_cast<int>(path.size()) / 2;

    return {
        path[middle - 1],
        path[middle]
    };
}

pair<int, int> findOffRouteEdge(
    const Graph& graph,
    const vector<int>& path
) {
    vector<pair<int, int>> edges =
        getAllEdges(graph);

    for (const auto& edge : edges) {
        if (!isRouteEdge(
                path,
                edge.first,
                edge.second
            )) {
            return edge;
        }
    }

    return {-1, -1};
}

bool validatePath(
    const Graph& graph,
    const vector<int>& path,
    int source,
    int destination
) {
    if (path.empty()) {
        return false;
    }

    if (path.front() != source ||
        path.back() != destination) {
        return false;
    }

    const vector<Node>& nodes =
        graph.getNodes();

    for (int i = 0; i + 1 < static_cast<int>(path.size()); i++) {
        int from = path[i];
        int to = path[i + 1];

        bool found = false;

        for (const auto& edge : nodes[from].edges) {
            if (edge.destination == to &&
                !edge.blocked) {
                found = true;
                break;
            }
        }

        if (!found) {
            return false;
        }
    }

    return true;
}

void printPath(
    const vector<int>& path
) {
    for (int node : path) {
        cout << node << " ";
    }

    cout << "\n";
}

void testInitialCorrectness(
    int rows,
    int cols
) {
    cout << "\n========================================\n";
    cout << "TEST 1: INITIAL CORRECTNESS\n";
    cout << "========================================\n";

    Graph graph =
        GraphGenerator::generateGrid(
            rows,
            cols
        );

    int source = 0;
    int destination = rows * cols - 1;

    LPAStar lpaStar;
    AStar aStar;

    RouteResult lpaResult =
        lpaStar.findShortestPath(
            graph,
            source,
            destination
        );

    RouteResult aStarResult =
        aStar.findShortestPath(
            graph,
            source,
            destination
        );

    cout << "LPA* cost: "
         << lpaResult.totalTime
         << "\n";

    cout << "A* cost: "
         << aStarResult.totalTime
         << "\n";

    cout << "LPA* nodes: "
         << lpaResult.nodesExplored
         << "\n";

    cout << "A* nodes: "
         << aStarResult.nodesExplored
         << "\n";

    cout << "LPA* path: ";
    printPath(lpaResult.path);

    if (lpaResult.totalTime ==
        aStarResult.totalTime) {

        cout << "PASS: Same optimal cost.\n";
    } else {
        cout << "FAIL: Different optimal cost.\n";
    }

    if (validatePath(
            graph,
            lpaResult.path,
            source,
            destination
        )) {

        cout << "PASS: LPA* path is valid.\n";
    } else {
        cout << "FAIL: LPA* path is invalid.\n";
    }
}

void testOnRouteUpdate(
    int rows,
    int cols
) {
    cout << "\n========================================\n";
    cout << "TEST 2: ON-ROUTE TRAFFIC UPDATE\n";
    cout << "========================================\n";

    Graph graph =
        GraphGenerator::generateGrid(
            rows,
            cols
        );

    int source = 0;
    int destination = rows * cols - 1;

    LPAStar lpaStar;
    AStar aStar;

    RouteResult initial =
        lpaStar.findShortestPath(
            graph,
            source,
            destination
        );

    if (initial.path.size() < 2) {
        cout << "FAIL: Initial route unavailable.\n";
        return;
    }

    pair<int, int> update =
        findOnRouteEdge(initial.path);

    int from = update.first;
    int to = update.second;

    cout << "Initial path: ";
    printPath(initial.path);

    cout << "Updating route edge: "
         << from
         << " -> "
         << to
         << "\n";

    graph.increaseTraffic(
        from,
        to
    );

    int before =
        lpaStar.getNodesExplored();

    auto lpaStart =
        high_resolution_clock::now();

    lpaStar.updateEdge(
        graph,
        from,
        to
    );

    lpaStar.computeShortestPath(
        graph
    );

    auto lpaEnd =
        high_resolution_clock::now();

    int after =
        lpaStar.getNodesExplored();

    long long lpaTime =
        duration_cast<microseconds>(
            lpaEnd - lpaStart
        ).count();

    int lpaUpdateNodes =
        after - before;

    auto aStarStart =
        high_resolution_clock::now();

    RouteResult aStarResult =
        aStar.findShortestPath(
            graph,
            source,
            destination
        );

    auto aStarEnd =
        high_resolution_clock::now();

    long long aStarTime =
        duration_cast<microseconds>(
            aStarEnd - aStarStart
        ).count();

    RouteResult lpaResult =
        lpaStar.getCurrentPath(
            graph
        );

    cout << "LPA* new cost: "
         << lpaResult.totalTime
         << "\n";

    cout << "A* new cost: "
         << aStarResult.totalTime
         << "\n";

    cout << "LPA* update time: "
         << lpaTime
         << " microseconds\n";

    cout << "A* recomputation time: "
         << aStarTime
         << " microseconds\n";

    cout << "LPA* update nodes: "
         << lpaUpdateNodes
         << "\n";

    cout << "A* nodes: "
         << aStarResult.nodesExplored
         << "\n";

    cout << "LPA* path: ";
    printPath(lpaResult.path);

    cout << "A* path: ";
    printPath(aStarResult.path);

    if (lpaResult.totalTime ==
        aStarResult.totalTime) {

        cout << "PASS: Same optimal cost.\n";
    } else {
        cout << "FAIL: Different optimal cost.\n";
    }

    if (lpaTime < aStarTime) {
        cout << "PASS: LPA* update was faster.\n";
    } else {
        cout << "INFO: LPA* was not faster in this run.\n";
    }

    if (lpaUpdateNodes <
        aStarResult.nodesExplored) {

        cout << "PASS: LPA* explored fewer nodes.\n";
    } else {
        cout << "INFO: LPA* explored the same or more nodes.\n";
    }
}

void testOffRouteUpdate(
    int rows,
    int cols
) {
    cout << "\n========================================\n";
    cout << "TEST 3: OFF-ROUTE TRAFFIC UPDATE\n";
    cout << "========================================\n";

    Graph graph =
        GraphGenerator::generateGrid(
            rows,
            cols
        );

    int source = 0;
    int destination = rows * cols - 1;

    LPAStar lpaStar;
    AStar aStar;

    RouteResult initial =
        lpaStar.findShortestPath(
            graph,
            source,
            destination
        );

    pair<int, int> update =
        findOffRouteEdge(
            graph,
            initial.path
        );

    if (update.first == -1) {
        cout << "FAIL: Could not find off-route edge.\n";
        return;
    }

    int from = update.first;
    int to = update.second;

    cout << "Off-route edge: "
         << from
         << " -> "
         << to
         << "\n";

    graph.increaseTraffic(
        from,
        to
    );

    int before =
        lpaStar.getNodesExplored();

    auto lpaStart =
        high_resolution_clock::now();

    lpaStar.updateEdge(
        graph,
        from,
        to
    );

    lpaStar.computeShortestPath(
        graph
    );

    auto lpaEnd =
        high_resolution_clock::now();

    int after =
        lpaStar.getNodesExplored();

    long long lpaTime =
        duration_cast<microseconds>(
            lpaEnd - lpaStart
        ).count();

    int lpaUpdateNodes =
        after - before;

    auto aStarStart =
        high_resolution_clock::now();

    RouteResult aStarResult =
        aStar.findShortestPath(
            graph,
            source,
            destination
        );

    auto aStarEnd =
        high_resolution_clock::now();

    long long aStarTime =
        duration_cast<microseconds>(
            aStarEnd - aStarStart
        ).count();

    RouteResult lpaResult =
        lpaStar.getCurrentPath(
            graph
        );

    cout << "LPA* cost: "
         << lpaResult.totalTime
         << "\n";

    cout << "A* cost: "
         << aStarResult.totalTime
         << "\n";

    cout << "LPA* update time: "
         << lpaTime
         << " microseconds\n";

    cout << "A* recomputation time: "
         << aStarTime
         << " microseconds\n";

    cout << "LPA* update nodes: "
         << lpaUpdateNodes
         << "\n";

    if (lpaResult.totalTime ==
        aStarResult.totalTime) {

        cout << "PASS: Same optimal cost.\n";
    } else {
        cout << "FAIL: Different optimal cost.\n";
    }

    if (lpaUpdateNodes <=
        aStarResult.nodesExplored) {

        cout << "PASS: LPA* did not need full recomputation.\n";
    } else {
        cout << "INFO: LPA* explored more nodes.\n";
    }
}

void testMultipleUpdates(
    int rows,
    int cols,
    int numberOfUpdates
) {
    cout << "\n========================================\n";
    cout << "TEST 4: MULTIPLE CONSECUTIVE UPDATES\n";
    cout << "========================================\n";

    Graph graph =
        GraphGenerator::generateGrid(
            rows,
            cols
        );

    int source = 0;
    int destination = rows * cols - 1;

    LPAStar lpaStar;
    AStar aStar;

    RouteResult initial =
        lpaStar.findShortestPath(
            graph,
            source,
            destination
        );

    vector<int> currentPath =
        initial.path;

    mt19937 rng(777);

    vector<pair<int, int>> edges =
        getAllEdges(graph);

    shuffle(
        edges.begin(),
        edges.end(),
        rng
    );

    long long totalLPA = 0;
    long long totalAStar = 0;

    int successfulUpdates = 0;

    for (
        int i = 0;
        i < numberOfUpdates &&
        i < static_cast<int>(edges.size());
        i++
    ) {
        int from = edges[i].first;
        int to = edges[i].second;

        graph.increaseTraffic(
            from,
            to
        );

        auto lpaStart =
            high_resolution_clock::now();

        lpaStar.updateEdge(
            graph,
            from,
            to
        );

        int before =
            lpaStar.getNodesExplored();

        lpaStar.computeShortestPath(
            graph
        );

        int after =
            lpaStar.getNodesExplored();

        auto lpaEnd =
            high_resolution_clock::now();

        long long lpaTime =
            duration_cast<microseconds>(
                lpaEnd - lpaStart
            ).count();

        auto aStarStart =
            high_resolution_clock::now();

        RouteResult aStarResult =
            aStar.findShortestPath(
                graph,
                source,
                destination
            );

        auto aStarEnd =
            high_resolution_clock::now();

        long long aStarTime =
            duration_cast<microseconds>(
                aStarEnd - aStarStart
            ).count();

        RouteResult lpaResult =
            lpaStar.getCurrentPath(
                graph
            );

        totalLPA += lpaTime;
        totalAStar += aStarTime;

        currentPath =
            lpaResult.path;

        bool correct =
            lpaResult.totalTime ==
            aStarResult.totalTime;

        cout << "Update "
             << i + 1
             << " | "
             << from
             << " -> "
             << to
             << " | LPA* "
             << lpaTime
             << " us | A* "
             << aStarTime
             << " us | Nodes "
             << after - before
             << " | ";

        if (correct) {
            cout << "PASS\n";
            successfulUpdates++;
        } else {
            cout << "FAIL\n";
        }
    }

    cout << "\nSuccessful updates: "
         << successfulUpdates
         << " / "
         << numberOfUpdates
         << "\n";

    cout << "Total LPA* update time: "
         << totalLPA
         << " microseconds\n";

    cout << "Total A* recomputation time: "
         << totalAStar
         << " microseconds\n";

    if (totalLPA < totalAStar) {
        cout << "PASS: LPA* faster over multiple updates.\n";
    } else {
        cout << "INFO: LPA* not faster over this sample.\n";
    }
}

void benchmarkGraph(
    int rows,
    int cols,
    int trials
) {
    cout << "\n========================================\n";
    cout << "TEST 5: PERFORMANCE BENCHMARK\n";
    cout << "========================================\n";

    cout << "Graph: "
         << rows
         << " x "
         << cols
         << "\n";

    cout << "Trials: "
         << trials
         << "\n";

    long long totalInitialLPA = 0;
    long long totalLPAUpdate = 0;
    long long totalAStar = 0;

    long long totalInitialNodes = 0;
    long long totalLPAUpdateNodes = 0;
    long long totalAStarNodes = 0;

    int validTrials = 0;

    for (int trial = 0; trial < trials; trial++) {
        Graph graph =
            GraphGenerator::generateGrid(
                rows,
                cols
            );

        int source = 0;
        int destination = rows * cols - 1;

        LPAStar lpaStar;
        AStar aStar;

        auto initialStart =
            high_resolution_clock::now();

        RouteResult initial =
            lpaStar.findShortestPath(
                graph,
                source,
                destination
            );

        auto initialEnd =
            high_resolution_clock::now();

        if (initial.path.empty()) {
            continue;
        }

        long long initialTime =
            duration_cast<microseconds>(
                initialEnd - initialStart
            ).count();

        pair<int, int> update =
            findOnRouteEdge(
                initial.path
            );

        if (update.first == -1) {
            continue;
        }

        graph.increaseTraffic(
            update.first,
            update.second
        );

        int before =
            lpaStar.getNodesExplored();

        auto lpaStart =
            high_resolution_clock::now();

        lpaStar.updateEdge(
            graph,
            update.first,
            update.second
        );

        lpaStar.computeShortestPath(
            graph
        );

        auto lpaEnd =
            high_resolution_clock::now();

        int after =
            lpaStar.getNodesExplored();

        long long lpaTime =
            duration_cast<microseconds>(
                lpaEnd - lpaStart
            ).count();

        int lpaNodes =
            after - before;

        auto aStarStart =
            high_resolution_clock::now();

        RouteResult aStarResult =
            aStar.findShortestPath(
                graph,
                source,
                destination
            );

        auto aStarEnd =
            high_resolution_clock::now();

        long long aStarTime =
            duration_cast<microseconds>(
                aStarEnd - aStarStart
            ).count();

        totalInitialLPA += initialTime;
        totalLPAUpdate += lpaTime;
        totalAStar += aStarTime;

        totalInitialNodes +=
            initial.nodesExplored;

        totalLPAUpdateNodes +=
            lpaNodes;

        totalAStarNodes +=
            aStarResult.nodesExplored;

        validTrials++;
    }

    if (validTrials == 0) {
        cout << "No valid trials.\n";
        return;
    }

    double averageInitialLPA =
        static_cast<double>(
            totalInitialLPA
        ) / validTrials;

    double averageLPAUpdate =
        static_cast<double>(
            totalLPAUpdate
        ) / validTrials;

    double averageAStar =
        static_cast<double>(
            totalAStar
        ) / validTrials;

    double averageInitialNodes =
        static_cast<double>(
            totalInitialNodes
        ) / validTrials;

    double averageLPAUpdateNodes =
        static_cast<double>(
            totalLPAUpdateNodes
        ) / validTrials;

    double averageAStarNodes =
        static_cast<double>(
            totalAStarNodes
        ) / validTrials;

    double timeReduction =
        100.0 *
        (averageAStar -
         averageLPAUpdate) /
        averageAStar;

    double nodeReduction =
        100.0 *
        (averageAStarNodes -
         averageLPAUpdateNodes) /
        averageAStarNodes;

    cout << "\nValid trials: "
         << validTrials
         << "\n";

    cout << "Average initial LPA* time: "
         << averageInitialLPA
         << " microseconds\n";

    cout << "Average LPA* update time: "
         << averageLPAUpdate
         << " microseconds\n";

    cout << "Average A* recomputation time: "
         << averageAStar
         << " microseconds\n";

    cout << "Average initial LPA* nodes: "
         << averageInitialNodes
         << "\n";

    cout << "Average LPA* update nodes: "
         << averageLPAUpdateNodes
         << "\n";

    cout << "Average A* nodes: "
         << averageAStarNodes
         << "\n";

    cout << "LPA* time reduction: "
         << timeReduction
         << "%\n";

    cout << "LPA* node reduction: "
         << nodeReduction
         << "%\n";
}

int main() {
    cout << "\n========================================\n";
    cout << "LPA* TEST SUITE\n";
    cout << "========================================\n";

    testInitialCorrectness(
        10,
        10
    );

    testOnRouteUpdate(
        10,
        10
    );

    testOffRouteUpdate(
        10,
        10
    );

    testMultipleUpdates(
        20,
        20,
        10
    );

    benchmarkGraph(
        10,
        10,
        20
    );

    benchmarkGraph(
        32,
        32,
        20
    );

    benchmarkGraph(
        50,
        50,
        20
    );

    benchmarkGraph(
        100,
        100,
        10
    );

    cout << "\n========================================\n";
    cout << "ALL LPA* TESTS COMPLETED\n";
    cout << "========================================\n";

    return 0;
}