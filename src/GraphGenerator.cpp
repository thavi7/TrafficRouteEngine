#include "GraphGenerator.h"

Graph GraphGenerator::generateGrid(int rows, int cols) {
    Graph graph(rows * cols);

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int node = row * cols + col;

            graph.setNodePosition(node, col, row);

            if (col + 1 < cols) {
                int right = row * cols + (col + 1);
                graph.addEdge(node, right, 1);
                graph.addEdge(right, node, 1);
            }

            if (row + 1 < rows) {
                int down = (row + 1) * cols + col;
                graph.addEdge(node, down, 1);
                graph.addEdge(down, node, 1);
            }
        }
    }

    return graph;
}