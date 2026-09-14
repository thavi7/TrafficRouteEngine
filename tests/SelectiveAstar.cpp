
// #include <iostream>
// #include <vector>
// #include <random>
// #include <chrono>
// #include "GraphGenerator.h"
// #include "Dijkstra.h"
// #include "AStar.h"

// using namespace std;
// using namespace chrono;

// bool isRouteEdge(const vector<int>& path, int from, int to) {
//     for (int i = 0; i + 1 < static_cast<int>(path.size()); i++) {
//         if ((path[i] == from && path[i + 1] == to) ||
//             (path[i] == to && path[i + 1] == from)) {
//             return true;
//         }
//     }

//     return false;
// }

// vector<pair<int, int>> generateUpdates(
//     const vector<pair<int, int>>& edges,
//     int numberOfUpdates,
//     unsigned int seed
// ) {
//     mt19937 rng(seed);
//     uniform_int_distribution<int> dist(
//         0,
//         static_cast<int>(edges.size()) - 1
//     );

//     vector<pair<int, int>> updates;

//     for (int i = 0; i < numberOfUpdates; i++) {
//         updates.push_back(edges[dist(rng)]);
//     }

//     return updates;
// }

// void runBenchmark(
//     int rows,
//     int cols,
//     int numberOfUpdates,
//     int trials
// ) {
//     vector<pair<int, int>> edges;

//     Graph baseGraph =
//         GraphGenerator::generateGrid(rows, cols);

//     const vector<Node>& nodes =
//         baseGraph.getNodes();

//     for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
//         for (const auto& edge : nodes[i].edges) {
//             if (i < edge.destination) {
//                 edges.push_back({
//                     i,
//                     edge.destination
//                 });
//             }
//         }
//     }

//     if (edges.empty()) {
//         cout << "No edges available.\n";
//         return;
//     }

//     int source = 0;
//     int destination = rows * cols - 1;

//     long long totalNaiveTime = 0;
//     long long totalSelectiveTime = 0;

//     long long totalNaiveDijkstraRuns = 0;
//     long long totalNaiveAStarRuns = 0;

//     long long totalSelectiveDijkstraRuns = 0;
//     long long totalSelectiveAStarRuns = 0;

//     long long totalSkippedUpdates = 0;

//     for (int trial = 0; trial < trials; trial++) {
//         vector<pair<int, int>> updates =
//             generateUpdates(
//                 edges,
//                 numberOfUpdates,
//                 1000 + trial
//             );

//         Graph graphNaive = baseGraph;
//         Graph graphSelective = baseGraph;

//         Dijkstra dijkstra;
//         AStar aStar;

//         auto naiveStart =
//             high_resolution_clock::now();

//         for (const auto& update : updates) {
//             int from = update.first;
//             int to = update.second;

//             graphNaive.increaseTraffic(
//                 from,
//                 to
//             );

//             dijkstra.findShortestPath(
//                 graphNaive,
//                 source,
//                 destination
//             );

//             aStar.findShortestPath(
//                 graphNaive,
//                 source,
//                 destination
//             );

//             totalNaiveDijkstraRuns++;
//             totalNaiveAStarRuns++;
//         }

//         auto naiveEnd =
//             high_resolution_clock::now();

//         totalNaiveTime +=
//             duration_cast<microseconds>(
//                 naiveEnd - naiveStart
//             ).count();

//         RouteResult initialRoute =
//             aStar.findShortestPath(
//                 graphSelective,
//                 source,
//                 destination
//             );

//         vector<int> currentPath =
//             initialRoute.path;

//         auto selectiveStart =
//             high_resolution_clock::now();

//         for (const auto& update : updates) {
//             int from = update.first;
//             int to = update.second;

//             graphSelective.increaseTraffic(
//                 from,
//                 to
//             );

//             if (isRouteEdge(
//                     currentPath,
//                     from,
//                     to
//                 )) {

//                 dijkstra.findShortestPath(
//                     graphSelective,
//                     source,
//                     destination
//                 );

//                 RouteResult aStarResult =
//                     aStar.findShortestPath(
//                         graphSelective,
//                         source,
//                         destination
//                     );

//                 currentPath =
//                     aStarResult.path;

//                 totalSelectiveDijkstraRuns++;
//                 totalSelectiveAStarRuns++;
//             } else {
//                 totalSkippedUpdates++;
//             }
//         }

//         auto selectiveEnd =
//             high_resolution_clock::now();

//         totalSelectiveTime +=
//             duration_cast<microseconds>(
//                 selectiveEnd - selectiveStart
//             ).count();
//     }

//     double averageNaiveTime =
//         static_cast<double>(
//             totalNaiveTime
//         ) / trials;

//     double averageSelectiveTime =
//         static_cast<double>(
//             totalSelectiveTime
//         ) / trials;

//     double averageNaiveDijkstra =
//         static_cast<double>(
//             totalNaiveDijkstraRuns
//         ) / trials;

//     double averageNaiveAStar =
//         static_cast<double>(
//             totalNaiveAStarRuns
//         ) / trials;

//     double averageSelectiveDijkstra =
//         static_cast<double>(
//             totalSelectiveDijkstraRuns
//         ) / trials;

//     double averageSelectiveAStar =
//         static_cast<double>(
//             totalSelectiveAStarRuns
//         ) / trials;

//     double averageSkipped =
//         static_cast<double>(
//             totalSkippedUpdates
//         ) / trials;

//     double timeReduction =
//         100.0 *
//         (averageNaiveTime -
//          averageSelectiveTime) /
//         averageNaiveTime;

//     double aStarReduction =
//         100.0 *
//         (averageNaiveAStar -
//          averageSelectiveAStar) /
//         averageNaiveAStar;

//     cout << "\n========================================\n";
//     cout << "Graph: "
//          << rows
//          << " x "
//          << cols
//          << "\n";

//     cout << "Nodes: "
//          << rows * cols
//          << "\n";

//     cout << "Traffic updates per trial: "
//          << numberOfUpdates
//          << "\n";

//     cout << "Trials: "
//          << trials
//          << "\n\n";

//     cout << "NAIVE\n";

//     cout << "Average Dijkstra runs: "
//          << averageNaiveDijkstra
//          << "\n";

//     cout << "Average A* runs: "
//          << averageNaiveAStar
//          << "\n";

//     cout << "Average time: "
//          << averageNaiveTime
//          << " microseconds\n\n";

//     cout << "SELECTIVE\n";

//     cout << "Average Dijkstra runs: "
//          << averageSelectiveDijkstra
//          << "\n";

//     cout << "Average A* runs: "
//          << averageSelectiveAStar
//          << "\n";

//     cout << "Average skipped updates: "
//          << averageSkipped
//          << "\n";

//     cout << "Average time: "
//          << averageSelectiveTime
//          << " microseconds\n\n";

//     cout << "Time reduction: "
//          << timeReduction
//          << "%\n";

//     cout << "A* execution reduction: "
//          << aStarReduction
//          << "%\n";
// }

// int main() {
//     const int numberOfUpdates = 1000;
//     const int trials = 5;

//     runBenchmark(10, 10, numberOfUpdates, trials);
//     runBenchmark(32, 32, numberOfUpdates, trials);
//     runBenchmark(50, 50, numberOfUpdates, trials);
//     runBenchmark(100, 100, numberOfUpdates, trials);

//     return 0;
// }

//added lpa* too

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "GraphGenerator.h"
#include "Dijkstra.h"
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
        updates.push_back(edges[dist(rng)]);
    }

    return updates;
}

void runBenchmark(
    int rows,
    int cols,
    int numberOfUpdates,
    int trials
) {
    vector<pair<int, int>> edges;

    Graph baseGraph =
        GraphGenerator::generateGrid(rows, cols);

    const vector<Node>& nodes =
        baseGraph.getNodes();

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

    if (edges.empty()) {
        cout << "No edges available.\n";
        return;
    }

    int source = 0;
    int destination = rows * cols - 1;

    long long totalNaiveTime = 0;
    long long totalSelectiveTime = 0;
    long long totalLPATime = 0;

    long long totalNaiveDijkstraRuns = 0;
    long long totalNaiveAStarRuns = 0;

    long long totalSelectiveDijkstraRuns = 0;
    long long totalSelectiveAStarRuns = 0;

    long long totalLPANodes = 0;
    long long totalLPAInitialNodes = 0;

    long long totalNaiveAStarNodes = 0;
    long long totalSelectiveAStarNodes = 0;

    long long totalSkippedUpdates = 0;

    long long totalLPAUpdateNodes = 0;

    long long totalNaiveUpdates = 0;
    long long totalSelectiveUpdates = 0;
    long long totalLPAUpdates = 0;

    int successfulTrials = 0;

    for (int trial = 0; trial < trials; trial++) {
        vector<pair<int, int>> updates =
            generateUpdates(
                edges,
                numberOfUpdates,
                1000 + trial
            );

        Graph graphNaive = baseGraph;
        Graph graphSelective = baseGraph;
        Graph graphLPA = baseGraph;

        Dijkstra dijkstraNaive;
        AStar aStarNaive;

        Dijkstra dijkstraSelective;
        AStar aStarSelective;

        AStar aStarLPA;
        LPAStar lpaStar;

        RouteResult initialLPA =
            lpaStar.findShortestPath(
                graphLPA,
                source,
                destination
            );

        RouteResult initialAStar =
            aStarLPA.findShortestPath(
                graphLPA,
                source,
                destination
            );

        if (initialLPA.path.empty() ||
            initialAStar.path.empty()) {
            continue;
        }

        totalLPAInitialNodes +=
            initialLPA.nodesExplored;

        auto naiveStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            int from = update.first;
            int to = update.second;

            graphNaive.increaseTraffic(
                from,
                to
            );

            dijkstraNaive.findShortestPath(
                graphNaive,
                source,
                destination
            );

            RouteResult result =
                aStarNaive.findShortestPath(
                    graphNaive,
                    source,
                    destination
                );

            totalNaiveAStarNodes +=
                result.nodesExplored;

            totalNaiveDijkstraRuns++;
            totalNaiveAStarRuns++;
            totalNaiveUpdates++;
        }

        auto naiveEnd =
            high_resolution_clock::now();

        totalNaiveTime +=
            duration_cast<microseconds>(
                naiveEnd - naiveStart
            ).count();

        RouteResult initialSelective =
            aStarSelective.findShortestPath(
                graphSelective,
                source,
                destination
            );

        vector<int> currentPath =
            initialSelective.path;

        auto selectiveStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            int from = update.first;
            int to = update.second;

            graphSelective.increaseTraffic(
                from,
                to
            );

            if (isRouteEdge(
                    currentPath,
                    from,
                    to
                )) {

                dijkstraSelective.findShortestPath(
                    graphSelective,
                    source,
                    destination
                );

                RouteResult aStarResult =
                    aStarSelective.findShortestPath(
                        graphSelective,
                        source,
                        destination
                    );

                currentPath =
                    aStarResult.path;

                totalSelectiveAStarNodes +=
                    aStarResult.nodesExplored;

                totalSelectiveDijkstraRuns++;
                totalSelectiveAStarRuns++;
            } else {
                totalSkippedUpdates++;
            }

            totalSelectiveUpdates++;
        }

        auto selectiveEnd =
            high_resolution_clock::now();

        totalSelectiveTime +=
            duration_cast<microseconds>(
                selectiveEnd - selectiveStart
            ).count();

        RouteResult lpaInitial =
            lpaStar.getCurrentPath(
                graphLPA
            );

        vector<int> lpaCurrentPath =
            lpaInitial.path;

        auto lpaStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            int from = update.first;
            int to = update.second;

            graphLPA.increaseTraffic(
                from,
                to
            );

            lpaStar.updateEdge(
                graphLPA,
                from,
                to
            );

            int before =
                lpaStar.getNodesExplored();

            lpaStar.computeShortestPath(
                graphLPA
            );

            int after =
                lpaStar.getNodesExplored();

            totalLPAUpdateNodes +=
                after - before;

            RouteResult lpaResult =
                lpaStar.getCurrentPath(
                    graphLPA
                );

            lpaCurrentPath =
                lpaResult.path;

            totalLPAUpdates++;
        }

        auto lpaEnd =
            high_resolution_clock::now();

        totalLPATime +=
            duration_cast<microseconds>(
                lpaEnd - lpaStart
            ).count();

        RouteResult finalLPA =
            lpaStar.getCurrentPath(
                graphLPA
            );

        RouteResult finalAStar =
            aStarLPA.findShortestPath(
                graphLPA,
                source,
                destination
            );

        if (finalLPA.totalTime !=
            finalAStar.totalTime) {
            cout << "Trial "
                 << trial + 1
                 << " failed validation.\n";
            continue;
        }

        successfulTrials++;
    }

    if (successfulTrials == 0) {
        cout << "No successful trials.\n";
        return;
    }

    double averageNaiveTime =
        static_cast<double>(
            totalNaiveTime
        ) / successfulTrials;

    double averageSelectiveTime =
        static_cast<double>(
            totalSelectiveTime
        ) / successfulTrials;

    double averageLPATime =
        static_cast<double>(
            totalLPATime
        ) / successfulTrials;

    double averageNaiveAStarRuns =
        static_cast<double>(
            totalNaiveAStarRuns
        ) / successfulTrials;

    double averageSelectiveAStarRuns =
        static_cast<double>(
            totalSelectiveAStarRuns
        ) / successfulTrials;

    double averageNaiveAStarNodes =
        static_cast<double>(
            totalNaiveAStarNodes
        ) / successfulTrials;

    double averageSelectiveAStarNodes =
        static_cast<double>(
            totalSelectiveAStarNodes
        ) / successfulTrials;

    double averageSkipped =
        static_cast<double>(
            totalSkippedUpdates
        ) / successfulTrials;

    double averageLPAInitialNodes =
        static_cast<double>(
            totalLPAInitialNodes
        ) / successfulTrials;

    double averageLPAUpdateNodes =
        static_cast<double>(
            totalLPAUpdateNodes
        ) / successfulTrials;

    double naiveToSelectiveReduction =
        100.0 *
        (averageNaiveTime -
         averageSelectiveTime) /
        averageNaiveTime;

    double naiveToLPAReduction =
        100.0 *
        (averageNaiveTime -
         averageLPATime) /
        averageNaiveTime;

    double aStarExecutionReduction =
        100.0 *
        (averageNaiveAStarRuns -
         averageSelectiveAStarRuns) /
        averageNaiveAStarRuns;

    double aStarNodeReduction =
        100.0 *
        (averageNaiveAStarNodes -
         averageSelectiveAStarNodes) /
        averageNaiveAStarNodes;

    double lpaNodeReduction =
        100.0 *
        (averageNaiveAStarNodes -
         averageLPAUpdateNodes) /
        averageNaiveAStarNodes;

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
    cout << "NAIVE APPROACH\n";
    cout << "========================================\n";

    cout << "Dijkstra runs: "
         << totalNaiveDijkstraRuns /
                successfulTrials
         << "\n";

    cout << "A* runs: "
         << averageNaiveAStarRuns
         << "\n";

    cout << "Average A* nodes explored: "
         << averageNaiveAStarNodes
         << "\n";

    cout << "Average total time: "
         << averageNaiveTime
         << " microseconds\n";

    cout << "\n========================================\n";
    cout << "SELECTIVE A* APPROACH\n";
    cout << "========================================\n";

    cout << "Dijkstra runs: "
         << totalSelectiveDijkstraRuns /
                successfulTrials
         << "\n";

    cout << "A* runs: "
         << averageSelectiveAStarRuns
         << "\n";

    cout << "Skipped updates: "
         << averageSkipped
         << "\n";

    cout << "Average A* nodes explored: "
         << averageSelectiveAStarNodes
         << "\n";

    cout << "Average total time: "
         << averageSelectiveTime
         << " microseconds\n";

    cout << "Time reduction vs naive: "
         << naiveToSelectiveReduction
         << "%\n";

    cout << "A* execution reduction: "
         << aStarExecutionReduction
         << "%\n";

    cout << "A* node reduction: "
         << aStarNodeReduction
         << "%\n";

    cout << "\n========================================\n";
    cout << "LPA* APPROACH\n";
    cout << "========================================\n";

    cout << "Average initial nodes explored: "
         << averageLPAInitialNodes
         << "\n";

    cout << "Average update nodes explored: "
         << averageLPAUpdateNodes
         << "\n";

    cout << "Average update time: "
         << averageLPATime
         << " microseconds\n";

    cout << "Time reduction vs naive: "
         << naiveToLPAReduction
         << "%\n";

    cout << "Node reduction vs A*: "
         << lpaNodeReduction
         << "%\n";

    cout << "\n========================================\n";
    cout << "COMPARISON\n";
    cout << "========================================\n";

    cout << "Naive A*:       "
         << averageNaiveTime
         << " us\n";

    cout << "Selective A*:   "
         << averageSelectiveTime
         << " us\n";

    cout << "LPA* updates:   "
         << averageLPATime
         << " us\n";

    if (averageSelectiveTime <
        averageNaiveTime) {

        cout << "PASS: Selective A* is faster than naive.\n";
    } else {
        cout << "FAIL: Selective A* is not faster.\n";
    }

    if (averageLPATime <
        averageNaiveTime) {

        cout << "PASS: LPA* updates are faster than naive.\n";
    } else {
        cout << "FAIL: LPA* updates are not faster.\n";
    }

    if (averageLPAUpdateNodes <
        averageNaiveAStarNodes) {

        cout << "PASS: LPA* explores fewer nodes during updates.\n";
    } else {
        cout << "FAIL: LPA* node exploration is not lower.\n";
    }

    cout << "\n========================================\n";
    cout << "BENCHMARK COMPLETED\n";
    cout << "========================================\n";
}

int main() {
    const int numberOfUpdates = 1000;
    const int trials = 5;

    runBenchmark(
        10,
        10,
        numberOfUpdates,
        trials
    );

    runBenchmark(
        32,
        32,
        numberOfUpdates,
        trials
    );

    runBenchmark(
        50,
        50,
        numberOfUpdates,
        trials
    );

    runBenchmark(
        100,
        100,
        numberOfUpdates,
        trials
    );

    return 0;
}