#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SFML/Graphics.hpp>
#include "Graph.h"
#include "RouteResult.h"

using namespace std;

class Visualizer {
private:
    sf::RenderWindow window;
    sf::Font font;

    bool isRouteEdge(const vector<int>& path, int from, int to);
    bool routeAffected(const vector<int>& path, int from, int to);
    int getClickedNode(const Graph& graph, sf::Vector2i mousePosition);
    pair<int, int> getClickedRoad(const Graph& graph, sf::Vector2i mousePosition);
    sf::Color getTrafficColor(const Edge& edge);

    void calculateRoute(
        Graph& graph,
        int source,
        int destination,
        RouteResult& route
    );

public:
    Visualizer(int width, int height, const string& title);

    void run(Graph& graph);

    bool isOpen();
};

#endif