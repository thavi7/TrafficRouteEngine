#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SFML/Graphics.hpp>
#include "Graph.h"
#include "RouteResult.h"
#include "LPAStar.h"

using namespace std;

class Visualizer {
private:
    sf::RenderWindow window;
    sf::Font font;

    LPAStar lpaStar;
    bool lpaInitialized = false;

    bool isRouteEdge(
        const vector<int>& path,
        int from,
        int to
    );

    bool routeAffected(
        const vector<int>& path,
        int from,
        int to
    );

    int getClickedNode(
        const Graph& graph,
        sf::Vector2i mousePosition
    );

    pair<int, int> getClickedRoad(
        const Graph& graph,
        sf::Vector2i mousePosition
    );

    sf::Color getTrafficColor(
        const Edge& edge
    );

    void calculateInitialRoute(
        Graph& graph,
        int source,
        int destination,
        RouteResult& route
    );

    void repairRoute(
        Graph& graph,
        int source,
        int destination,
        int from,
        int to,
        RouteResult& route
    );

public:
    Visualizer(
        int width,
        int height,
        const string& title
    );

    void run(Graph& graph);

    bool isOpen();
};

#endif