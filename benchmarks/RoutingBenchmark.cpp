#include <iostream>
#include <chrono>
#include "GraphGenerator.h"
#include "Dijkstra.h"
#include "AStar.h"

using namespace std;
using namespace chrono;

int main() {
    Graph graph = GraphGenerator::generateGrid(100, 100);

    int source = 100;
    int destination = 9999;

    Dijkstra dijkstra;
    AStar aStar;

    const int runs = 100;

    long long totalDijkstraTime = 0;
    long long totalAStarTime = 0;

    RouteResult dijkstraResult;
    RouteResult aStarResult;

    for (int i = 0; i < runs; i++) {
        auto start = high_resolution_clock::now();

        dijkstraResult = dijkstra.findShortestPath(graph, source, destination);

        auto end = high_resolution_clock::now();

        totalDijkstraTime +=
            duration_cast<microseconds>(end - start).count();
    }

    for (int i = 0; i < runs; i++) {
        auto start = high_resolution_clock::now();

        aStarResult = aStar.findShortestPath(graph, source, destination);

        auto end = high_resolution_clock::now();

        totalAStarTime +=
            duration_cast<microseconds>(end - start).count();
    }

    cout << "Dijkstra:\n";
    cout << "Travel time: " << dijkstraResult.totalTime << "\n";
    cout << "Nodes explored: " << dijkstraResult.nodesExplored << "\n";
    cout << "Average time: "
         << totalDijkstraTime / runs << " microseconds\n\n";

    cout << "A*:\n";
    cout << "Travel time: " << aStarResult.totalTime << "\n";
    cout << "Nodes explored: " << aStarResult.nodesExplored << "\n";
    cout << "Average time: "
         << totalAStarTime / runs << " microseconds\n";

    return 0;
}