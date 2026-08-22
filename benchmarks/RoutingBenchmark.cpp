// #include <iostream>
// #include <chrono>
// #include "GraphGenerator.h"
// #include "Dijkstra.h"
// #include "AStar.h"

// using namespace std;
// using namespace chrono;

// int main() {
//     Graph graph = GraphGenerator::generateGrid(100, 100);

//     int source = 100;
//     int destination = 9999;

//     Dijkstra dijkstra;
//     AStar aStar;

//     const int runs = 100;

//     long long totalDijkstraTime = 0;
//     long long totalAStarTime = 0;

//     RouteResult dijkstraResult;
//     RouteResult aStarResult;

//     for (int i = 0; i < runs; i++) {
//         auto start = high_resolution_clock::now();

//         dijkstraResult = dijkstra.findShortestPath(graph, source, destination);

//         auto end = high_resolution_clock::now();

//         totalDijkstraTime +=
//             duration_cast<microseconds>(end - start).count();
//     }

//     for (int i = 0; i < runs; i++) {
//         auto start = high_resolution_clock::now();

//         aStarResult = aStar.findShortestPath(graph, source, destination);

//         auto end = high_resolution_clock::now();

//         totalAStarTime +=
//             duration_cast<microseconds>(end - start).count();
//     }

//     cout << "Dijkstra:\n";
//     cout << "Travel time: " << dijkstraResult.totalTime << "\n";
//     cout << "Nodes explored: " << dijkstraResult.nodesExplored << "\n";
//     cout << "Average time: "
//          << totalDijkstraTime / runs << " microseconds\n\n";

//     cout << "A*:\n";
//     cout << "Travel time: " << aStarResult.totalTime << "\n";
//     cout << "Nodes explored: " << aStarResult.nodesExplored << "\n";
//     cout << "Average time: "
//          << totalAStarTime / runs << " microseconds\n";

//     return 0;
// }
// #include <iostream>
// #include <vector>
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

// int main() {
//     const int rows = 100;
//     const int cols = 100;
//     const int numberOfUpdates = 1000;

//     Graph graphNaive = GraphGenerator::generateGrid(rows, cols);
//     Graph graphSelective = graphNaive;

//     int source = 0;
//     int destination = rows * cols - 1;

//     Dijkstra dijkstra;
//     AStar aStar;

//     vector<pair<int, int>> edges;

//     const vector<Node>& nodes = graphNaive.getNodes();

//     for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
//         for (const auto& edge : nodes[i].edges) {
//             if (i < edge.destination) {
//                 edges.push_back({i, edge.destination});
//             }
//         }
//     }

//     if (edges.empty()) {
//         cout << "No edges available.\n";
//         return 0;
//     }

//     vector<pair<int, int>> testUpdates;

//     for (int i = 0; i < numberOfUpdates; i++) {
//         testUpdates.push_back(
//             edges[i % edges.size()]
//         );
//     }

//     cout << "Graph: "
//          << rows << " x " << cols
//          << "\n";

//     cout << "Nodes: "
//          << rows * cols
//          << "\n";

//     cout << "Traffic updates: "
//          << numberOfUpdates
//          << "\n\n";

//     RouteResult initialSelective =
//         aStar.findShortestPath(
//             graphSelective,
//             source,
//             destination
//         );

//     int naiveDijkstraRuns = 0;
//     int naiveAStarRuns = 0;

//     auto naiveStart = high_resolution_clock::now();

//     for (const auto& update : testUpdates) {
//         int from = update.first;
//         int to = update.second;

//         graphNaive.increaseTraffic(from, to);

//         dijkstra.findShortestPath(
//             graphNaive,
//             source,
//             destination
//         );

//         aStar.findShortestPath(
//             graphNaive,
//             source,
//             destination
//         );

//         naiveDijkstraRuns++;
//         naiveAStarRuns++;
//     }

//     auto naiveEnd = high_resolution_clock::now();

//     auto naiveTime =
//         duration_cast<microseconds>(
//             naiveEnd - naiveStart
//         ).count();

//     int selectiveDijkstraRuns = 0;
//     int selectiveAStarRuns = 0;
//     int skippedUpdates = 0;

//     vector<int> currentPath =
//         initialSelective.path;

//     auto selectiveStart = high_resolution_clock::now();

//     for (const auto& update : testUpdates) {
//         int from = update.first;
//         int to = update.second;

//         graphSelective.increaseTraffic(from, to);

//         if (isRouteEdge(currentPath, from, to)) {
//             RouteResult dijkstraResult =
//                 dijkstra.findShortestPath(
//                     graphSelective,
//                     source,
//                     destination
//                 );

//             RouteResult aStarResult =
//                 aStar.findShortestPath(
//                     graphSelective,
//                     source,
//                     destination
//                 );

//             currentPath = aStarResult.path;

//             selectiveDijkstraRuns++;
//             selectiveAStarRuns++;
//         } else {
//             skippedUpdates++;
//         }
//     }

//     auto selectiveEnd = high_resolution_clock::now();

//     auto selectiveTime =
//         duration_cast<microseconds>(
//             selectiveEnd - selectiveStart
//         ).count();

//     cout << "===== NAIVE =====\n";

//     cout << "Dijkstra runs: "
//          << naiveDijkstraRuns
//          << "\n";

//     cout << "A* runs: "
//          << naiveAStarRuns
//          << "\n";

//     cout << "Total time: "
//          << naiveTime
//          << " microseconds\n\n";

//     cout << "===== SELECTIVE =====\n";

//     cout << "Dijkstra runs: "
//          << selectiveDijkstraRuns
//          << "\n";

//     cout << "A* runs: "
//          << selectiveAStarRuns
//          << "\n";

//     cout << "Skipped updates: "
//          << skippedUpdates
//          << "\n";

//     cout << "Total time: "
//          << selectiveTime
//          << " microseconds\n\n";

//     if (naiveTime > 0) {
//         double improvement =
//             100.0 *
//             (static_cast<double>(naiveTime - selectiveTime) /
//              naiveTime);

//         cout << "Time reduction: "
//              << improvement
//              << "%\n";
//     }

//     if (naiveAStarRuns > 0) {
//         double runReduction =
//             100.0 *
//             (static_cast<double>(naiveAStarRuns - selectiveAStarRuns) /
//              naiveAStarRuns);

//         cout << "A* execution reduction: "
//              << runReduction
//              << "%\n";
//     }

//     return 0;
// }

//testing for more

// #include <iostream>
// #include <vector>
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

// void runBenchmark(int rows, int cols, int numberOfUpdates) {
//     Graph graphNaive = GraphGenerator::generateGrid(rows, cols);
//     Graph graphSelective = graphNaive;

//     int source = 0;
//     int destination = rows * cols - 1;

//     Dijkstra dijkstra;
//     AStar aStar;

//     vector<pair<int, int>> edges;

//     const vector<Node>& nodes = graphNaive.getNodes();

//     for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
//         for (const auto& edge : nodes[i].edges) {
//             if (i < edge.destination) {
//                 edges.push_back({i, edge.destination});
//             }
//         }
//     }

//     if (edges.empty()) {
//         cout << "No edges available.\n";
//         return;
//     }

//     vector<pair<int, int>> testUpdates;

//     for (int i = 0; i < numberOfUpdates; i++) {
//         testUpdates.push_back(
//             edges[i % edges.size()]
//         );
//     }

//     RouteResult initialSelective =
//         aStar.findShortestPath(
//             graphSelective,
//             source,
//             destination
//         );

//     int naiveDijkstraRuns = 0;
//     int naiveAStarRuns = 0;

//     auto naiveStart = high_resolution_clock::now();

//     for (const auto& update : testUpdates) {
//         int from = update.first;
//         int to = update.second;

//         graphNaive.increaseTraffic(from, to);

//         dijkstra.findShortestPath(
//             graphNaive,
//             source,
//             destination
//         );

//         aStar.findShortestPath(
//             graphNaive,
//             source,
//             destination
//         );

//         naiveDijkstraRuns++;
//         naiveAStarRuns++;
//     }

//     auto naiveEnd = high_resolution_clock::now();

//     auto naiveTime =
//         duration_cast<microseconds>(
//             naiveEnd - naiveStart
//         ).count();

//     int selectiveDijkstraRuns = 0;
//     int selectiveAStarRuns = 0;
//     int skippedUpdates = 0;

//     vector<int> currentPath =
//         initialSelective.path;

//     auto selectiveStart = high_resolution_clock::now();

//     for (const auto& update : testUpdates) {
//         int from = update.first;
//         int to = update.second;

//         graphSelective.increaseTraffic(from, to);

//         if (isRouteEdge(currentPath, from, to)) {
//             RouteResult dijkstraResult =
//                 dijkstra.findShortestPath(
//                     graphSelective,
//                     source,
//                     destination
//                 );

//             RouteResult aStarResult =
//                 aStar.findShortestPath(
//                     graphSelective,
//                     source,
//                     destination
//                 );

//             currentPath = aStarResult.path;

//             selectiveDijkstraRuns++;
//             selectiveAStarRuns++;
//         } else {
//             skippedUpdates++;
//         }
//     }

//     auto selectiveEnd = high_resolution_clock::now();

//     auto selectiveTime =
//         duration_cast<microseconds>(
//             selectiveEnd - selectiveStart
//         ).count();

//     double timeReduction = 0.0;
//     double astarReduction = 0.0;

//     if (naiveTime > 0) {
//         timeReduction =
//             100.0 *
//             (static_cast<double>(naiveTime - selectiveTime) /
//              naiveTime);
//     }

//     if (naiveAStarRuns > 0) {
//         astarReduction =
//             100.0 *
//             (static_cast<double>(
//                 naiveAStarRuns - selectiveAStarRuns) /
//              naiveAStarRuns);
//     }

//     cout << "\n========================================\n";
//     cout << "Graph: "
//          << rows
//          << " x "
//          << cols
//          << "\n";

//     cout << "Nodes: "
//          << rows * cols
//          << "\n";

//     cout << "Traffic updates: "
//          << numberOfUpdates
//          << "\n\n";

//     cout << "NAIVE\n";
//     cout << "Dijkstra runs: "
//          << naiveDijkstraRuns
//          << "\n";

//     cout << "A* runs: "
//          << naiveAStarRuns
//          << "\n";

//     cout << "Time: "
//          << naiveTime
//          << " microseconds\n\n";

//     cout << "SELECTIVE\n";
//     cout << "Dijkstra runs: "
//          << selectiveDijkstraRuns
//          << "\n";

//     cout << "A* runs: "
//          << selectiveAStarRuns
//          << "\n";

//     cout << "Skipped updates: "
//          << skippedUpdates
//          << "\n";

//     cout << "Time: "
//          << selectiveTime
//          << " microseconds\n\n";

//     cout << "Time reduction: "
//          << timeReduction
//          << "%\n";

//     cout << "A* execution reduction: "
//          << astarReduction
//          << "%\n";
// }

// int main() {
//     runBenchmark(10, 10, 1000);
//     runBenchmark(32, 32, 1000);
//     runBenchmark(50, 50, 1000);
//     runBenchmark(100, 100, 1000);

//     return 0;
// }

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "GraphGenerator.h"
#include "Dijkstra.h"
#include "AStar.h"

using namespace std;
using namespace chrono;

bool isRouteEdge(const vector<int>& path, int from, int to) {
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

    long long totalNaiveDijkstraRuns = 0;
    long long totalNaiveAStarRuns = 0;

    long long totalSelectiveDijkstraRuns = 0;
    long long totalSelectiveAStarRuns = 0;

    long long totalSkippedUpdates = 0;

    for (int trial = 0; trial < trials; trial++) {
        vector<pair<int, int>> updates =
            generateUpdates(
                edges,
                numberOfUpdates,
                1000 + trial
            );

        Graph graphNaive = baseGraph;
        Graph graphSelective = baseGraph;

        Dijkstra dijkstra;
        AStar aStar;

        auto naiveStart =
            high_resolution_clock::now();

        for (const auto& update : updates) {
            int from = update.first;
            int to = update.second;

            graphNaive.increaseTraffic(
                from,
                to
            );

            dijkstra.findShortestPath(
                graphNaive,
                source,
                destination
            );

            aStar.findShortestPath(
                graphNaive,
                source,
                destination
            );

            totalNaiveDijkstraRuns++;
            totalNaiveAStarRuns++;
        }

        auto naiveEnd =
            high_resolution_clock::now();

        totalNaiveTime +=
            duration_cast<microseconds>(
                naiveEnd - naiveStart
            ).count();

        RouteResult initialRoute =
            aStar.findShortestPath(
                graphSelective,
                source,
                destination
            );

        vector<int> currentPath =
            initialRoute.path;

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

                dijkstra.findShortestPath(
                    graphSelective,
                    source,
                    destination
                );

                RouteResult aStarResult =
                    aStar.findShortestPath(
                        graphSelective,
                        source,
                        destination
                    );

                currentPath =
                    aStarResult.path;

                totalSelectiveDijkstraRuns++;
                totalSelectiveAStarRuns++;
            } else {
                totalSkippedUpdates++;
            }
        }

        auto selectiveEnd =
            high_resolution_clock::now();

        totalSelectiveTime +=
            duration_cast<microseconds>(
                selectiveEnd - selectiveStart
            ).count();
    }

    double averageNaiveTime =
        static_cast<double>(
            totalNaiveTime
        ) / trials;

    double averageSelectiveTime =
        static_cast<double>(
            totalSelectiveTime
        ) / trials;

    double averageNaiveDijkstra =
        static_cast<double>(
            totalNaiveDijkstraRuns
        ) / trials;

    double averageNaiveAStar =
        static_cast<double>(
            totalNaiveAStarRuns
        ) / trials;

    double averageSelectiveDijkstra =
        static_cast<double>(
            totalSelectiveDijkstraRuns
        ) / trials;

    double averageSelectiveAStar =
        static_cast<double>(
            totalSelectiveAStarRuns
        ) / trials;

    double averageSkipped =
        static_cast<double>(
            totalSkippedUpdates
        ) / trials;

    double timeReduction =
        100.0 *
        (averageNaiveTime -
         averageSelectiveTime) /
        averageNaiveTime;

    double aStarReduction =
        100.0 *
        (averageNaiveAStar -
         averageSelectiveAStar) /
        averageNaiveAStar;

    cout << "\n========================================\n";
    cout << "Graph: "
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
         << "\n\n";

    cout << "NAIVE\n";

    cout << "Average Dijkstra runs: "
         << averageNaiveDijkstra
         << "\n";

    cout << "Average A* runs: "
         << averageNaiveAStar
         << "\n";

    cout << "Average time: "
         << averageNaiveTime
         << " microseconds\n\n";

    cout << "SELECTIVE\n";

    cout << "Average Dijkstra runs: "
         << averageSelectiveDijkstra
         << "\n";

    cout << "Average A* runs: "
         << averageSelectiveAStar
         << "\n";

    cout << "Average skipped updates: "
         << averageSkipped
         << "\n";

    cout << "Average time: "
         << averageSelectiveTime
         << " microseconds\n\n";

    cout << "Time reduction: "
         << timeReduction
         << "%\n";

    cout << "A* execution reduction: "
         << aStarReduction
         << "%\n";
}

int main() {
    const int numberOfUpdates = 1000;
    const int trials = 5;

    runBenchmark(10, 10, numberOfUpdates, trials);
    runBenchmark(32, 32, numberOfUpdates, trials);
    runBenchmark(50, 50, numberOfUpdates, trials);
    runBenchmark(100, 100, numberOfUpdates, trials);

    return 0;
}