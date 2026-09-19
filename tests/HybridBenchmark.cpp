#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <fstream>
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

vector<pair<int, int>> getEdges(
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

vector<pair<int, int>> generateUpdates(
    const vector<pair<int, int>>& edges,
    int numberOfUpdates,
    unsigned int seed
) {
    mt19937 rng(seed);

    uniform_int_distribution<int> dist(
        0,
        static_cast<int>(edges.size()) - 1
    );

    vector<pair<int, int>> updates;

    for (int i = 0; i < numberOfUpdates; i++) {
        updates.push_back(
            edges[dist(rng)]
        );
    }

    return updates;
}

void writeCSVHeader() {
    ifstream checkFile("benchmark_results.csv");

    if (checkFile.good()) {
        checkFile.close();
        return;
    }

    checkFile.close();

    ofstream file("benchmark_results.csv");

    file << "graph_size,rows,cols,algorithm,runtime_us,nodes_explored,reroutes,skipped_updates,time_reduction_percent\n";

    file.close();
}

void writeCSVRow(
    int graphSize,
    int rows,
    int cols,
    const string& algorithm,
    double runtime,
    double nodesExplored,
    double reroutes,
    double skippedUpdates,
    double timeReduction
) {
    ofstream file(
        "benchmark_results.csv",
        ios::app
    );

    file << graphSize << ","
         << rows << ","
         << cols << ","
         << algorithm << ","
         << fixed << setprecision(2)
         << runtime << ","
         << nodesExplored << ","
         << reroutes << ","
         << skippedUpdates << ","
         << timeReduction
         << "\n";

    file.close();
}

void runBenchmark(
    int rows,
    int cols,
    int numberOfUpdates,
    int trials
) {
    Graph baseGraph =
        GraphGenerator::generateGrid(
            rows,
            cols
        );

    vector<pair<int, int>> edges =
        getEdges(baseGraph);

    int source = 0;
    int destination =
        rows * cols - 1;

    long long naiveTime = 0;
    long long selectiveTime = 0;
    long long lpaTime = 0;
    long long hybridTime = 0;

    long long naiveNodes = 0;
    long long selectiveNodes = 0;
    long long lpaUpdateNodes = 0;
    long long hybridNodes = 0;

    long long selectiveRuns = 0;
    long long selectiveSkipped = 0;

    long long lpaUpdates = 0;
    long long hybridUpdates = 0;
    long long hybridSkipped = 0;

    long long successfulTrials = 0;

    for (int trial = 0; trial < trials; trial++) {
        vector<pair<int, int>> updates =
            generateUpdates(
                edges,
                numberOfUpdates,
                1000 + trial
            );

        Graph naiveGraph =
            baseGraph;

        Graph selectiveGraph =
            baseGraph;

        Graph lpaGraph =
            baseGraph;

        Graph hybridGraph =
            baseGraph;

        AStar naiveAStar;
        AStar selectiveAStar;
        AStar lpaReferenceAStar;
        AStar hybridReferenceAStar;

        LPAStar lpa;
        LPAStar hybridLPA;

        auto naiveStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            naiveGraph.increaseTraffic(
                update.first,
                update.second
            );

            RouteResult result =
                naiveAStar.findShortestPath(
                    naiveGraph,
                    source,
                    destination
                );

            naiveNodes +=
                result.nodesExplored;
        }

        auto naiveEnd =
            high_resolution_clock::now();

        naiveTime +=
            duration_cast<microseconds>(
                naiveEnd - naiveStart
            ).count();

        RouteResult selectiveInitial =
            selectiveAStar.findShortestPath(
                selectiveGraph,
                source,
                destination
            );

        vector<int> selectivePath =
            selectiveInitial.path;

        auto selectiveStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            selectiveGraph.increaseTraffic(
                update.first,
                update.second
            );

            if (isRouteEdge(
                    selectivePath,
                    update.first,
                    update.second
                )) {

                RouteResult result =
                    selectiveAStar.findShortestPath(
                        selectiveGraph,
                        source,
                        destination
                    );

                selectivePath =
                    result.path;

                selectiveNodes +=
                    result.nodesExplored;

                selectiveRuns++;
            } else {
                selectiveSkipped++;
            }
        }

        auto selectiveEnd =
            high_resolution_clock::now();

        selectiveTime +=
            duration_cast<microseconds>(
                selectiveEnd - selectiveStart
            ).count();

        RouteResult lpaInitial =
            lpa.findShortestPath(
                lpaGraph,
                source,
                destination
            );

        auto lpaStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            lpaGraph.increaseTraffic(
                update.first,
                update.second
            );

            lpa.updateEdge(
                lpaGraph,
                update.first,
                update.second
            );

            int before =
                lpa.getNodesExplored();

            lpa.computeShortestPath(
                lpaGraph
            );

            int after =
                lpa.getNodesExplored();

            lpaUpdateNodes +=
                after - before;

            lpaUpdates++;
        }

        auto lpaEnd =
            high_resolution_clock::now();

        lpaTime +=
            duration_cast<microseconds>(
                lpaEnd - lpaStart
            ).count();

        RouteResult hybridInitial =
            hybridLPA.findShortestPath(
                hybridGraph,
                source,
                destination
            );

        vector<int> hybridPath =
            hybridInitial.path;

        auto hybridStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            int from = update.first;
            int to = update.second;

            hybridGraph.increaseTraffic(
                from,
                to
            );

            if (!isRouteEdge(
                    hybridPath,
                    from,
                    to
                )) {

                hybridSkipped++;

                continue;
            }

            int before =
                hybridLPA.getNodesExplored();

            hybridLPA.updateEdge(
                hybridGraph,
                from,
                to
            );

            hybridLPA.computeShortestPath(
                hybridGraph
            );

            int after =
                hybridLPA.getNodesExplored();

            hybridNodes +=
                after - before;

            RouteResult updated =
                hybridLPA.getCurrentPath(
                    hybridGraph
                );

            hybridPath =
                updated.path;

            hybridUpdates++;
        }

        auto hybridEnd =
            high_resolution_clock::now();

        hybridTime +=
            duration_cast<microseconds>(
                hybridEnd - hybridStart
            ).count();

        successfulTrials++;
    }

    double avgNaiveTime =
        static_cast<double>(
            naiveTime
        ) / successfulTrials;

    double avgSelectiveTime =
        static_cast<double>(
            selectiveTime
        ) / successfulTrials;

    double avgLPATime =
        static_cast<double>(
            lpaTime
        ) / successfulTrials;

    double avgHybridTime =
        static_cast<double>(
            hybridTime
        ) / successfulTrials;

    double avgNaiveNodes =
        static_cast<double>(
            naiveNodes
        ) / successfulTrials;

    double avgSelectiveNodes =
        static_cast<double>(
            selectiveNodes
        ) / successfulTrials;

    double avgLPANodes =
        static_cast<double>(
            lpaUpdateNodes
        ) / successfulTrials;

    double avgHybridNodes =
        static_cast<double>(
            hybridNodes
        ) / successfulTrials;

    double avgSelectiveRuns =
        static_cast<double>(
            selectiveRuns
        ) / successfulTrials;

    double avgSelectiveSkipped =
        static_cast<double>(
            selectiveSkipped
        ) / successfulTrials;

    double avgLPARuns =
        static_cast<double>(
            lpaUpdates
        ) / successfulTrials;

    double avgHybridRuns =
        static_cast<double>(
            hybridUpdates
        ) / successfulTrials;

    double avgHybridSkipped =
        static_cast<double>(
            hybridSkipped
        ) / successfulTrials;

    double selectiveReduction =
        100.0 *
        (avgNaiveTime - avgSelectiveTime) /
        avgNaiveTime;

    double lpaReduction =
        100.0 *
        (avgNaiveTime - avgLPATime) /
        avgNaiveTime;

    double hybridReduction =
        100.0 *
        (avgNaiveTime - avgHybridTime) /
        avgNaiveTime;

    double hybridVsSelective =
        100.0 *
        (avgSelectiveTime - avgHybridTime) /
        avgSelectiveTime;

    writeCSVRow(
        rows * cols,
        rows,
        cols,
        "Naive A*",
        avgNaiveTime,
        avgNaiveNodes,
        numberOfUpdates,
        0,
        0
    );

    writeCSVRow(
        rows * cols,
        rows,
        cols,
        "Selective A*",
        avgSelectiveTime,
        avgSelectiveNodes,
        avgSelectiveRuns,
        avgSelectiveSkipped,
        selectiveReduction
    );

    writeCSVRow(
        rows * cols,
        rows,
        cols,
        "LPA*",
        avgLPATime,
        avgLPANodes,
        avgLPARuns,
        0,
        lpaReduction
    );

    writeCSVRow(
        rows * cols,
        rows,
        cols,
        "Hybrid",
        avgHybridTime,
        avgHybridNodes,
        avgHybridRuns,
        avgHybridSkipped,
        hybridReduction
    );

    cout << "\n========================================\n";
    cout << "GRAPH: "
         << rows
         << " x "
         << cols
         << "\n";

    cout << "Nodes: "
         << rows * cols
         << "\n";

    cout << "Traffic updates per trial: "
         << numberOfUpdates
         << "\n";

    cout << "Trials: "
         << trials
         << "\n";

    cout << "Successful trials: "
         << successfulTrials
         << "\n";

    cout << "\n========================================\n";
    cout << "NAIVE A*\n";
    cout << "========================================\n";

    cout << "A* runs: "
         << numberOfUpdates
         << "\n";

    cout << "Average A* nodes: "
         << avgNaiveNodes
         << "\n";

    cout << "Average time: "
         << avgNaiveTime
         << " us\n";

    cout << "\n========================================\n";
    cout << "SELECTIVE A*\n";
    cout << "========================================\n";

    cout << "A* runs: "
         << avgSelectiveRuns
         << "\n";

    cout << "Skipped updates: "
         << avgSelectiveSkipped
         << "\n";

    cout << "Average A* nodes: "
         << avgSelectiveNodes
         << "\n";

    cout << "Average time: "
         << avgSelectiveTime
         << " us\n";

    cout << "Time reduction vs naive: "
         << selectiveReduction
         << "%\n";

    cout << "\n========================================\n";
    cout << "LPA*\n";
    cout << "========================================\n";

    cout << "LPA* updates: "
         << avgLPARuns
         << "\n";

    cout << "Average update nodes: "
         << avgLPANodes
         << "\n";

    cout << "Average update time: "
         << avgLPATime
         << " us\n";

    cout << "Time reduction vs naive: "
         << lpaReduction
         << "%\n";

    cout << "\n========================================\n";
    cout << "HYBRID: SELECTIVE + LPA*\n";
    cout << "========================================\n";

    cout << "LPA* updates: "
         << avgHybridRuns
         << "\n";

    cout << "Skipped updates: "
         << avgHybridSkipped
         << "\n";

    cout << "Average update nodes: "
         << avgHybridNodes
         << "\n";

    cout << "Average update time: "
         << avgHybridTime
         << " us\n";

    cout << "Time reduction vs naive: "
         << hybridReduction
         << "%\n";

    cout << "Improvement vs selective A*: "
         << hybridVsSelective
         << "%\n";

    cout << "\n========================================\n";
    cout << "COMPARISON\n";
    cout << "========================================\n";

    cout << fixed << setprecision(2);

    cout << "Naive A*:       "
         << avgNaiveTime
         << " us\n";

    cout << "Selective A*:   "
         << avgSelectiveTime
         << " us\n";

    cout << "LPA*:            "
         << avgLPATime
         << " us\n";

    cout << "Hybrid:          "
         << avgHybridTime
         << " us\n";

    if (avgSelectiveTime < avgNaiveTime) {
        cout << "PASS: Selective A* is faster than naive A*.\n";
    }

    if (avgLPATime < avgNaiveTime) {
        cout << "PASS: LPA* is faster than naive A*.\n";
    }

    if (avgHybridTime < avgNaiveTime) {
        cout << "PASS: Hybrid is faster than naive A*.\n";
    }

    if (avgHybridTime < avgSelectiveTime) {
        cout << "PASS: Hybrid is faster than Selective A*.\n";
    } else {
        cout << "INFO: Selective A* is faster than Hybrid on this workload.\n";
    }

    cout << "\n========================================\n";
    cout << "BENCHMARK COMPLETED\n";
    cout << "========================================\n";
}

int main() {
    writeCSVHeader();

    runBenchmark(10, 10, 1000, 5);
    runBenchmark(32, 32, 1000, 5);
    runBenchmark(50, 50, 1000, 5);
    runBenchmark(100, 100, 1000, 5);

    return 0;
}