#include "GraphGenerator.h"
#include "Visualizer.h"

int main() {
    Graph graph = GraphGenerator::generateGrid(10, 10);

    Visualizer visualizer(
        800,
        600,
        "Traffic Route Engine"
    );

    visualizer.run(graph);

    return 0;
}
// int main() {
//     Graph graph = GraphGenerator::generateGrid(10, 10);

//     int source = 0;
//     int destination = 99;

//     AStar aStar;
//     LPAStar lpaStar;

//     RouteResult initialLPA =
//         lpaStar.findShortestPath(
//             graph,
//             source,
//             destination
//         );

//     cout << "Initial LPA*:\n";
//     cout << "Time: "
//          << initialLPA.totalTime
//          << "\n";

//     cout << "Path: ";
//     printPath(initialLPA.path);

//     if (initialLPA.path.empty()) {
//         cout << "LPA* failed to find a path.\n";
//         return 0;
//     }

//     int middle =
//         initialLPA.path.size() / 2;

//     int from =
//         initialLPA.path[middle];

//     int to =
//         initialLPA.path[middle + 1];

//     cout << "\nIncreasing traffic on road: "
//          << from
//          << " -> "
//          << to
//          << "\n";

//     graph.increaseTraffic(
//         from,
//         to
//     );

//     lpaStar.updateEdge(
//         graph,
//         from,
//         to
//     );

//     lpaStar.computeShortestPath(
//         graph,
//         source,
//         destination
//     );

//     RouteResult updatedLPA =
//         lpaStar.findShortestPath(
//             graph,
//             source,
//             destination
//         );

//     RouteResult updatedAStar =
//         aStar.findShortestPath(
//             graph,
//             source,
//             destination
//         );

//     cout << "\nAfter traffic update:\n";

//     cout << "LPA* Time: "
//          << updatedLPA.totalTime
//          << "\n";

//     cout << "A* Time: "
//          << updatedAStar.totalTime
//          << "\n";

//     cout << "LPA* Path: ";
//     printPath(updatedLPA.path);

//     cout << "A* Path: ";
//     printPath(updatedAStar.path);

//     return 0;
// }
// #include "GraphGenerator.h"
// #include "AStar.h"
// #include "LPAStar.h"
// #include <iostream>

// using namespace std;

// int main() {
//     cout << "1. Program started" << endl;

//     Graph graph = GraphGenerator::generateGrid(10, 10);

//     cout << "2. Graph generated" << endl;

//     int source = 0;
//     int destination = 99;

//     LPAStar lpaStar;

//     cout << "3. LPA object created" << endl;

//     RouteResult result =
//         lpaStar.findShortestPath(
//             graph,
//             source,
//             destination
//         );

//     cout << "4. LPA finished" << endl;

//     cout << "Time: "
//          << result.totalTime
//          << endl;

//     cout << "Path size: "
//          << result.path.size()
//          << endl;

//     return 0;
// }