#include "OSMLoader.h"
#include "Visualizer.h"
#include <exception>
#include <iostream>

using namespace std;

int main() {
    try {
        cout << "Loading OSM map..." << endl;

        Graph graph = OSMLoader::load("data/map.osm");

        cout << "Map loaded!" << endl;
        cout << "Nodes: " << graph.getNodes().size() << endl;

        Visualizer visualizer(
            800,
            600,
            "Traffic Route Engine - OpenStreetMap"
        );

        cout << "Starting visualizer..." << endl;
        visualizer.run(graph);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}