
#include "Visualizer.h"
#include "AStar.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace {

int previousLpaNodes = 0;

struct AlgorithmStats {
    RouteResult result;
    double timeMs = 0.0;
    int nodesProcessed = 0;
    bool skipped = false;
};

struct MapTransform {
    float scale;
    float offsetX;
    float offsetY;
};

void printPath(const RouteResult& route) {
    cout << "Path: ";

    if (route.path.empty()) {
        cout << "No path";
    } else {
        for (int node : route.path) {
            cout << node << " ";
        }
    }

    cout << "\nTravel time: " << route.totalTime << "\n";
}

void printRouteResult(const string& name, const RouteResult& result) {
    cout << "\n========================================\n";
    cout << name << "\n";
    cout << "========================================\n";
    printPath(result);
    cout << "Nodes explored: " << result.nodesExplored << "\n";
    cout << "========================================\n";
}

template <typename Function>
AlgorithmStats measureAlgorithm(Function function) {
    auto start = chrono::high_resolution_clock::now();

    RouteResult result = function();

    auto end = chrono::high_resolution_clock::now();

    AlgorithmStats stats;
    stats.result = result;
    stats.timeMs =
        chrono::duration<double, milli>(end - start).count();
    stats.nodesProcessed = result.nodesExplored;

    return stats;
}

void printComparison(
    const AlgorithmStats& selective,
    const AlgorithmStats& hybrid
) {
    cout << "\n========================================\n";
    cout << "SELECTIVE A* + HYBRID LPA* COMPARISON\n";
    cout << "========================================\n";

    cout << "\nSELECTIVE A*\n";
    if (selective.skipped) {
        cout << "Current route not affected. Search skipped.\n";
    } else {
        cout << "Execution time: " << selective.timeMs << " ms\n";
        cout << "Nodes explored: " << selective.nodesProcessed << "\n";
    }
    printPath(selective.result);

    cout << "\nHYBRID LPA*\n";
    if (hybrid.skipped) {
        cout << "Current route not affected. Repair skipped.\n";
    } else {
        cout << "Repair time: " << hybrid.timeMs << " ms\n";
        cout << "Repair nodes processed: " << hybrid.nodesProcessed << "\n";
        cout << "Previous search state: Reused\n";
    }
    printPath(hybrid.result);

    cout << "========================================\n";
}

MapTransform getMapTransform(
    const Graph& graph,
    unsigned int width,
    unsigned int height
) {
    const auto& nodes = graph.getNodes();

    if (nodes.empty()) {
        return {1.0f, 0.0f, 0.0f};
    }

    double minX = nodes[0].x;
    double maxX = nodes[0].x;
    double minY = nodes[0].y;
    double maxY = nodes[0].y;

    for (const Node& node : nodes) {
        minX = min(minX, node.x);
        maxX = max(maxX, node.x);
        minY = min(minY, node.y);
        maxY = max(maxY, node.y);
    }

    double mapWidth = maxX - minX;
    double mapHeight = maxY - minY;

    if (mapWidth < 0.000001) {
        mapWidth = 1.0;
    }

    if (mapHeight < 0.000001) {
        mapHeight = 1.0;
    }

    float availableWidth =
        max(1.0f, static_cast<float>(width) - 60.0f);

    float availableHeight =
        max(1.0f, static_cast<float>(height) - 115.0f);

    float scaleX =
        availableWidth / static_cast<float>(mapWidth);

    float scaleY =
        availableHeight / static_cast<float>(mapHeight);

    float scale = min(scaleX, scaleY);

    float offsetX =
        (static_cast<float>(width) -
         static_cast<float>(mapWidth) * scale) / 2.0f
        - static_cast<float>(minX) * scale;

    float offsetY =
        75.0f +
        (availableHeight -
         static_cast<float>(mapHeight) * scale) / 2.0f
        - static_cast<float>(minY) * scale;

    return {scale, offsetX, offsetY};
}

sf::Vector2f toScreen(
    const Node& node,
    const MapTransform& transform
) {
    return {
        static_cast<float>(node.x) * transform.scale
            + transform.offsetX,
        static_cast<float>(node.y) * transform.scale
            + transform.offsetY
    };
}

float distanceToSegment(
    sf::Vector2f point,
    sf::Vector2f a,
    sf::Vector2f b
) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    float lengthSquared = dx * dx + dy * dy;

    if (lengthSquared < 0.0001f) {
        float px = point.x - a.x;
        float py = point.y - a.y;

        return sqrt(px * px + py * py);
    }

    float t =
        ((point.x - a.x) * dx +
         (point.y - a.y) * dy) / lengthSquared;

    t = max(0.0f, min(1.0f, t));

    float closestX = a.x + t * dx;
    float closestY = a.y + t * dy;

    float px = point.x - closestX;
    float py = point.y - closestY;

    return sqrt(px * px + py * py);
}

}

Visualizer::Visualizer(
    int width,
    int height,
    const string& title
)
    : window(
        sf::VideoMode(
            sf::Vector2u(
                static_cast<unsigned int>(width),
                static_cast<unsigned int>(height)
            )
        ),
        title
    ) {
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        cerr << "Warning: Could not load Arial font.\n";
    }
}

sf::Color Visualizer::getTrafficColor(const Edge& edge) {
    if (edge.blocked) {
        return sf::Color(65, 65, 65);
    }

    int increase = edge.travelTime - edge.baseTravelTime;

    if (increase <= 0) {
        return sf::Color(180, 180, 180);
    }

    if (increase <= 5) {
        return sf::Color(255, 255, 0);
    }

    if (increase <= 10) {
        return sf::Color(255, 165, 0);
    }

    return sf::Color(255, 60, 60);
}

bool Visualizer::isRouteEdge(
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

bool Visualizer::routeAffected(
    const vector<int>& path,
    int from,
    int to
) {
    return isRouteEdge(path, from, to);
}

int Visualizer::getClickedNode(
    const Graph& graph,
    sf::Vector2i mousePosition
) {
    const auto& nodes = graph.getNodes();
    auto size = window.getSize();

    MapTransform transform =
        getMapTransform(graph, size.x, size.y);

    sf::Vector2f mouse(
        static_cast<float>(mousePosition.x),
        static_cast<float>(mousePosition.y)
    );

    int selected = -1;
    float bestDistance = 10.0f;

    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        sf::Vector2f position = toScreen(nodes[i], transform);

        float dx = mouse.x - position.x;
        float dy = mouse.y - position.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance < bestDistance) {
            bestDistance = distance;
            selected = i;
        }
    }

    return selected;
}

pair<int, int> Visualizer::getClickedRoad(
    const Graph& graph,
    sf::Vector2i mousePosition
) {
    const auto& nodes = graph.getNodes();
    auto size = window.getSize();

    MapTransform transform =
        getMapTransform(graph, size.x, size.y);

    sf::Vector2f mouse(
        static_cast<float>(mousePosition.x),
        static_cast<float>(mousePosition.y)
    );

    float bestDistance = 12.0f;
    pair<int, int> selectedRoad = {-1, -1};

    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        sf::Vector2f a = toScreen(nodes[i], transform);

        for (const auto& edge : nodes[i].edges) {
            int j = edge.destination;

            if (j < 0 ||
                j >= static_cast<int>(nodes.size()) ||
                i >= j) {
                continue;
            }

            sf::Vector2f b = toScreen(nodes[j], transform);

            float distance = distanceToSegment(mouse, a, b);

            if (distance < bestDistance) {
                bestDistance = distance;
                selectedRoad = {i, j};
            }
        }
    }

    return selectedRoad;
}

void Visualizer::calculateInitialRoute(
    Graph& graph,
    int source,
    int destination,
    RouteResult& route
) {
    if (source < 0 ||
        destination < 0 ||
        source == destination) {
        return;
    }

    AStar aStar;

    AlgorithmStats selectiveStats = measureAlgorithm(
        [&]() {
            return aStar.findShortestPath(
                graph,
                source,
                destination
            );
        }
    );

    lpaStar.initialize(
        graph,
        source,
        destination
    );

    auto start = chrono::high_resolution_clock::now();

    lpaStar.computeShortestPath(graph);

    RouteResult hybridResult =
        lpaStar.getCurrentPath(graph);

    auto end = chrono::high_resolution_clock::now();

    AlgorithmStats hybridStats;
    hybridStats.result = hybridResult;
    hybridStats.timeMs =
        chrono::duration<double, milli>(end - start).count();
    hybridStats.nodesProcessed = hybridResult.nodesExplored;

    previousLpaNodes = hybridResult.nodesExplored;
    lpaInitialized = true;

    route = hybridResult;

    printRouteResult("SELECTIVE A*", selectiveStats.result);
    printRouteResult("HYBRID LPA*", hybridResult);

    cout << "\nInitial Hybrid LPA* computation time: "
         << hybridStats.timeMs << " ms\n";
}

void Visualizer::repairRoute(
    Graph& graph,
    int source,
    int destination,
    int from,
    int to,
    RouteResult& route
) {
    if (!lpaInitialized) {
        calculateInitialRoute(
            graph,
            source,
            destination,
            route
        );
        return;
    }

    lpaStar.updateEdge(graph, from, to);
    lpaStar.computeShortestPath(graph);

    route = lpaStar.getCurrentPath(graph);
}

void Visualizer::run(Graph& graph) {
    int source = -1;
    int destination = -1;
    int selectedFrom = -1;
    int selectedTo = -1;

    RouteResult route;
    RouteResult selectiveRoute;

    while (window.isOpen()) {
        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mouseButton =
                    event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePosition(
                        mouseButton->position.x,
                        mouseButton->position.y
                    );

                    int clickedNode =
                        getClickedNode(graph, mousePosition);

                    if (clickedNode != -1) {
                        if (source == -1) {
                            source = clickedNode;

                            cout << "Source selected: "
                                 << source << "\n";
                        } else if (destination == -1 &&
                                   clickedNode != source) {
                            destination = clickedNode;

                            cout << "Destination selected: "
                                 << destination << "\n";

                            calculateInitialRoute(
                                graph,
                                source,
                                destination,
                                route
                            );

                            AStar aStar;
                            selectiveRoute =
                                aStar.findShortestPath(
                                    graph,
                                    source,
                                    destination
                                );

                            printRouteResult(
                                "SELECTIVE A* INITIAL ROUTE",
                                selectiveRoute
                            );
                        }
                    } else if (source != -1 &&
                               destination != -1) {
                        pair<int, int> road =
                            getClickedRoad(graph, mousePosition);

                        if (road.first != -1) {
                            selectedFrom = road.first;
                            selectedTo = road.second;

                            cout << "\nSelected road: "
                                 << selectedFrom << " <-> "
                                 << selectedTo << "\n";
                        }
                    }
                }
            }

            if (const auto* key =
                    event->getIf<sf::Event::KeyPressed>()) {
                if (selectedFrom == -1 ||
                    selectedTo == -1 ||
                    source == -1 ||
                    destination == -1) {
                    continue;
                }

                bool increase =
                    key->code == sf::Keyboard::Key::Up;

                bool decrease =
                    key->code == sf::Keyboard::Key::Down;

                bool close =
                    key->code == sf::Keyboard::Key::C;

                bool open =
                    key->code == sf::Keyboard::Key::O;

                bool reset =
                    key->code == sf::Keyboard::Key::R;

                if (!increase && !decrease &&
                    !close && !open && !reset) {
                    continue;
                }

                bool selectiveAffected =
                    routeAffected(
                        selectiveRoute.path,
                        selectedFrom,
                        selectedTo
                    );

                bool hybridAffected =
                    routeAffected(
                        route.path,
                        selectedFrom,
                        selectedTo
                    );

                if (increase) {
                    graph.increaseTraffic(selectedFrom, selectedTo);
                    cout << "\nTraffic increased on road ";
                } else if (decrease) {
                    graph.decreaseTraffic(selectedFrom, selectedTo);
                    cout << "\nTraffic decreased on road ";
                } else if (close) {
                    graph.closeRoad(selectedFrom, selectedTo);
                    cout << "\nRoad closed: ";
                } else if (open) {
                    graph.openRoad(selectedFrom, selectedTo);
                    cout << "\nRoad opened: ";
                } else {
                    graph.resetTraffic(selectedFrom, selectedTo);
                    cout << "\nTraffic reset on road ";
                }

                cout << selectedFrom << " <-> "
                     << selectedTo << "\n";

                AlgorithmStats selectiveStats;
                AlgorithmStats hybridStats;

                bool forceRecalculation = open || reset || decrease;

                if (selectiveAffected || forceRecalculation) {
                    selectiveStats = measureAlgorithm(
                        [&]() {
                            AStar aStar;
                            return aStar.findShortestPath(
                                graph,
                                source,
                                destination
                            );
                        }
                    );

                    selectiveRoute = selectiveStats.result;
                } else {
                    selectiveStats.result = selectiveRoute;
                    selectiveStats.skipped = true;
                }

                auto hybridStart =
                    chrono::high_resolution_clock::now();

                if (hybridAffected || forceRecalculation) {
                    repairRoute(
                        graph,
                        source,
                        destination,
                        selectedFrom,
                        selectedTo,
                        route
                    );

                    auto hybridEnd =
                        chrono::high_resolution_clock::now();

                    hybridStats.result = route;
                    hybridStats.timeMs =
                        chrono::duration<double, milli>(
                            hybridEnd - hybridStart
                        ).count();

                    hybridStats.nodesProcessed =
                        max(
                            0,
                            route.nodesExplored - previousLpaNodes
                        );

                    previousLpaNodes = route.nodesExplored;
                } else {
                    hybridStats.result = route;
                    hybridStats.skipped = true;
                }

                if (close && !selectiveAffected) {
                    cout << "Selective A*: current route not affected. "
                            "Search skipped.\n";
                }

                if (close && !hybridAffected) {
                    cout << "Hybrid LPA*: current route not affected. "
                            "Repair skipped.\n";
                }

                printComparison(selectiveStats, hybridStats);
            }
        }

        window.clear(sf::Color(14, 18, 28));

        const auto& nodes = graph.getNodes();
        auto size = window.getSize();

        MapTransform transform =
            getMapTransform(graph, size.x, size.y);

        for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
            sf::Vector2f a = toScreen(nodes[i], transform);

            for (const auto& edge : nodes[i].edges) {
                int j = edge.destination;

                if (j < 0 ||
                    j >= static_cast<int>(nodes.size()) ||
                    i >= j) {
                    continue;
                }

                sf::Vector2f b = toScreen(nodes[j], transform);

                sf::Color roadColor = getTrafficColor(edge);

                sf::Vertex line[2];
                line[0].position = a;
                line[0].color = roadColor;
                line[1].position = b;
                line[1].color = roadColor;

                window.draw(
                    line,
                    2,
                    sf::PrimitiveType::Lines
                );

                bool selected =
                    (selectedFrom == i && selectedTo == j) ||
                    (selectedFrom == j && selectedTo == i);

                if (selected) {
                    sf::CircleShape marker(4.0f);
                    marker.setOrigin(sf::Vector2f(4.0f, 4.0f));
                    marker.setPosition((a + b) / 2.0f);
                    marker.setFillColor(sf::Color::Cyan);
                    window.draw(marker);
                }

                bool onHybridRoute =
                    isRouteEdge(route.path, i, j);

                if (onHybridRoute) {
                    sf::CircleShape marker(2.0f);
                    marker.setOrigin(sf::Vector2f(2.0f, 2.0f));
                    marker.setPosition((a + b) / 2.0f);
                    marker.setFillColor(sf::Color::White);
                    window.draw(marker);
                }
            }
        }

        for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
            sf::Vector2f position = toScreen(nodes[i], transform);

            float radius = (i == source || i == destination)
                ? 5.0f
                : 2.0f;

            sf::CircleShape circle(radius);
            circle.setOrigin(sf::Vector2f(radius, radius));
            circle.setPosition(position);

            if (i == source) {
                circle.setFillColor(sf::Color::Green);
            } else if (i == destination) {
                circle.setFillColor(sf::Color::Blue);
            } else {
                circle.setFillColor(sf::Color(210, 220, 235));
            }

            window.draw(circle);
        }

        if (font.getInfo().family != "") {
            sf::Text help(
                font,
                "Select source, destination, then click a road",
                14
            );

            help.setFillColor(sf::Color::White);
            help.setPosition(sf::Vector2f(12.0f, 8.0f));
            window.draw(help);

            sf::Text controls(
                font,
                "UP: traffic + | DOWN: traffic - | C: close | O: open | R: reset",
                12
            );

            controls.setFillColor(sf::Color(190, 200, 215));
            controls.setPosition(sf::Vector2f(12.0f, 30.0f));
            window.draw(controls);

            sf::Text legend(
                font,
                "Gray: Normal | Yellow: Level 1 | Orange: Level 2 | Red: Level 3 | Dark gray: Closed",
                12
            );

            legend.setFillColor(sf::Color(220, 225, 235));
            legend.setPosition(sf::Vector2f(12.0f, 50.0f));
            window.draw(legend);
        }

        window.display();
    }
}

bool Visualizer::isOpen() {
    return window.isOpen();
}
