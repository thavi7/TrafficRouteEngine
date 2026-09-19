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
