#include "Visualizer.h"
#include "Dijkstra.h"
#include "AStar.h"
#include <cmath>
#include <iostream>

Visualizer::Visualizer(int width, int height, const string& title)
    : window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title) {
    if (!font.openFromFile("assets/Arial.ttf")) {
        window.close();
    }
}

sf::Color Visualizer::getTrafficColor(const Edge& edge) {
    if (edge.blocked) {
        return sf::Color(60, 60, 60);
    }

    int increase = edge.travelTime - edge.baseTravelTime;

    if (increase <= 0) {
        return sf::Color(150, 150, 150);
    }

    if (increase <= 5) {
        return sf::Color::Yellow;
    }

    if (increase <= 10) {
        return sf::Color(255, 165, 0);
    }

    return sf::Color(180, 0, 180);
}

bool Visualizer::isOpen() {
    return window.isOpen();
}

bool Visualizer::isRouteEdge(const vector<int>& path, int from, int to) {
    for (int i = 0; i + 1 < static_cast<int>(path.size()); i++) {
        if ((path[i] == from && path[i + 1] == to) ||
            (path[i] == to && path[i + 1] == from)) {
            return true;
        }
    }

    return false;
}

int Visualizer::getClickedNode(
    const Graph& graph,
    sf::Vector2i mousePosition
) {
    const vector<Node>& nodes = graph.getNodes();

    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        float screenX = static_cast<float>(nodes[i].x * 50 + 100);
        float screenY = static_cast<float>(nodes[i].y * 50 + 100);

        float dx = mousePosition.x - screenX;
        float dy = mousePosition.y - screenY;

        if (dx * dx + dy * dy <= 12 * 12) {
            return i;
        }
    }

    return -1;
}

pair<int, int> Visualizer::getClickedRoad(
    const Graph& graph,
    sf::Vector2i mousePosition
) {
    const vector<Node>& nodes = graph.getNodes();

    float mouseX = static_cast<float>(mousePosition.x);
    float mouseY = static_cast<float>(mousePosition.y);

    float bestDistance = 12.0f;
    pair<int, int> selectedRoad = {-1, -1};

    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        for (const auto& edge : nodes[i].edges) {
            int j = edge.destination;

            if (i > j) {
                continue;
            }

            float x1 = static_cast<float>(nodes[i].x * 50 + 100);
            float y1 = static_cast<float>(nodes[i].y * 50 + 100);

            float x2 = static_cast<float>(nodes[j].x * 50 + 100);
            float y2 = static_cast<float>(nodes[j].y * 50 + 100);

            float dx = x2 - x1;
            float dy = y2 - y1;

            float lengthSquared = dx * dx + dy * dy;

            if (lengthSquared == 0) {
                continue;
            }

            float t = ((mouseX - x1) * dx + (mouseY - y1) * dy) / lengthSquared;

            t = max(0.0f, min(1.0f, t));

            float closestX = x1 + t * dx;
            float closestY = y1 + t * dy;

            float distanceX = mouseX - closestX;
            float distanceY = mouseY - closestY;

            float distance = sqrt(
                distanceX * distanceX +
                distanceY * distanceY
            );

            if (distance < bestDistance) {
                bestDistance = distance;
                selectedRoad = {i, j};
            }
        }
    }

    return selectedRoad;
}

void Visualizer::calculateRoute(
    Graph& graph,
    int source,
    int destination,
    RouteResult& route
) {
    if (source == -1 || destination == -1) {
        return;
    }

    Dijkstra dijkstra;
    AStar aStar;

    RouteResult dijkstraResult =
        dijkstra.findShortestPath(
            graph,
            source,
            destination
        );

    RouteResult aStarResult =
        aStar.findShortestPath(
            graph,
            source,
            destination
        );

    cout << "\nDijkstra:\n";
    cout << "Path: ";

    for (int node : dijkstraResult.path) {
        cout << node << " ";
    }

    cout << "\n";
    cout << "Total travel time: "
         << dijkstraResult.totalTime << "\n";
    cout << "Nodes explored: "
         << dijkstraResult.nodesExplored << "\n";

    cout << "\nA*:\n";
    cout << "Path: ";

    for (int node : aStarResult.path) {
        cout << node << " ";
    }

    cout << "\n";
    cout << "Total travel time: "
         << aStarResult.totalTime << "\n";
    cout << "Nodes explored: "
         << aStarResult.nodesExplored << "\n\n";

    route = aStarResult;
}

void Visualizer::run(Graph& graph) {
    int source = -1;
    int destination = -1;

    int selectedFrom = -1;
    int selectedTo = -1;

    RouteResult route;

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
                        } else if (
                            destination == -1 &&
                            clickedNode != source
                        ) {
                            destination = clickedNode;

                            cout << "Destination selected: "
                                 << destination << "\n";

                            calculateRoute(
                                graph,
                                source,
                                destination,
                                route
                            );
                        }
                    } else if (source != -1 && destination != -1) {
                        pair<int, int> road =
                            getClickedRoad(
                                graph,
                                mousePosition
                            );

                        if (road.first != -1) {
                            selectedFrom = road.first;
                            selectedTo = road.second;

                            cout << "\nSelected road: "
                                 << selectedFrom
                                 << " <-> "
                                 << selectedTo
                                 << "\n";
                        }
                    }
                }
            }

            if (const auto* key =
                    event->getIf<sf::Event::KeyPressed>()) {

                if (selectedFrom != -1 && selectedTo != -1 &&
                    source != -1 && destination != -1) {

                    if (key->code == sf::Keyboard::Key::Up) {
                        graph.increaseTraffic(
                            selectedFrom,
                            selectedTo
                        );

                        cout << "\nTraffic increased on road "
                             << selectedFrom
                             << " <-> "
                             << selectedTo
                             << "\n";

                        calculateRoute(
                            graph,
                            source,
                            destination,
                            route
                        );
                    }

                    if (key->code == sf::Keyboard::Key::Down) {
                        graph.decreaseTraffic(
                            selectedFrom,
                            selectedTo
                        );

                        cout << "\nTraffic decreased on road "
                             << selectedFrom
                             << " <-> "
                             << selectedTo
                             << "\n";

                        calculateRoute(
                            graph,
                            source,
                            destination,
                            route
                        );
                    }

                    if (key->code == sf::Keyboard::Key::R) {
                        graph.resetTraffic(
                            selectedFrom,
                            selectedTo
                        );

                        cout << "\nTraffic reset on road "
                             << selectedFrom
                             << " <-> "
                             << selectedTo
                             << "\n";

                        calculateRoute(
                            graph,
                            source,
                            destination,
                            route
                        );
                    }

                    if (key->code == sf::Keyboard::Key::C) {
                        graph.closeRoad(
                            selectedFrom,
                            selectedTo
                        );

                        cout << "\nRoad closed: "
                             << selectedFrom
                             << " <-> "
                             << selectedTo
                             << "\n";

                        calculateRoute(
                            graph,
                            source,
                            destination,
                            route
                        );
                    }

                    if (key->code == sf::Keyboard::Key::O) {
                        graph.openRoad(
                            selectedFrom,
                            selectedTo
                        );

                        cout << "\nRoad opened: "
                             << selectedFrom
                             << " <-> "
                             << selectedTo
                             << "\n";

                        calculateRoute(
                            graph,
                            source,
                            destination,
                            route
                        );
                    }
                }
            }
        }

        window.clear();

        const vector<Node>& nodes = graph.getNodes();

        for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
            const auto& node = nodes[i];

            for (const auto& edge : node.edges) {
                int destinationNodeId = edge.destination;

                if (i > destinationNodeId) {
                    continue;
                }

                const Node& destinationNode =
                    nodes[destinationNodeId];

                sf::Vertex line[2];

                line[0].position = sf::Vector2f(
                    static_cast<float>(node.x * 50 + 100),
                    static_cast<float>(node.y * 50 + 100)
                );

                line[1].position = sf::Vector2f(
                    static_cast<float>(destinationNode.x * 50 + 100),
                    static_cast<float>(destinationNode.y * 50 + 100)
                );

                if (i == selectedFrom && destinationNodeId == selectedTo) {
                    line[0].color = sf::Color::Cyan;
                    line[1].color = sf::Color::Cyan;
                } else if (edge.blocked) {
                    line[0].color = sf::Color(60, 60, 60);
                    line[1].color = sf::Color(60, 60, 60);
                } else if (isRouteEdge(route.path, i, destinationNodeId)) {
                    line[0].color = sf::Color::Red;
                    line[1].color = sf::Color::Red;
                } else {
                    sf::Color trafficColor = getTrafficColor(edge);
                    line[0].color = trafficColor;
                    line[1].color = trafficColor;
                }

                window.draw(
                    line,
                    2,
                    sf::PrimitiveType::Lines
                );
            }
        }

        for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
            const auto& node = nodes[i];

            sf::CircleShape circle(6);

            circle.setPosition({
                static_cast<float>(node.x * 50 + 94),
                static_cast<float>(node.y * 50 + 94)
            });

            if (i == source) {
                circle.setFillColor(sf::Color::Green);
            } else if (i == destination) {
                circle.setFillColor(sf::Color::Blue);
            } else {
                circle.setFillColor(sf::Color::White);
            }

            window.draw(circle);

            sf::Text text(font);

            text.setString(to_string(i));
            text.setCharacterSize(12);
            text.setFillColor(sf::Color::White);

            text.setPosition({
                static_cast<float>(node.x * 50 + 102),
                static_cast<float>(node.y * 50 + 90)
            });

            window.draw(text);
        }

        window.display();
    }
}

/*

🔴 Red = current route
🟪 Purple = heavy traffic
🟧 Orange = medium traffic
🟨 Yellow = low traffic
🟦 Cyan = selected road
⬛ Dark = closed


*/