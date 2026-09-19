#include "Visualizer.h"
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

void printPath(const RouteResult& route) {
    cout << "Path: ";

    if (route.path.empty()) {
        cout << "No path";
    } else {
        for (int node : route.path) {
            cout << node << " ";
        }
    }

    cout << "\n";

    cout << "Travel time: "
         << route.totalTime
         << "\n";
}

void printInitialRoute(
    const RouteResult& route,
    double timeMs
) {
    cout << "\n========================================\n";
    cout << "HYBRID LPA* ROUTING\n";
    cout << "========================================\n";

    cout << "Initial computation time: "
         << timeMs
         << " ms\n";

    cout << "Nodes processed: "
         << route.nodesExplored
         << "\n";

    printPath(route);

    cout << "========================================\n";
}

void printRepairResult(
    const RouteResult& route,
    double timeMs,
    int nodesProcessed
) {
    cout << "\n========================================\n";
    cout << "HYBRID LPA* UPDATE\n";
    cout << "========================================\n";

    cout << "Repair Time: "
         << timeMs
         << " ms\n";

    cout << "Repair Nodes Processed: "
         << nodesProcessed
         << "\n";

    cout << "Previous Search State: Reused\n";

    printPath(route);

    cout << "========================================\n";
}

void printNoRepair(
    const RouteResult& route
) {
    cout << "\n========================================\n";
    cout << "HYBRID LPA* UPDATE\n";
    cout << "========================================\n";

    cout << "Current route not affected.\n";
    cout << "No repair needed.\n";

    printPath(route);

    cout << "========================================\n";
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
                width,
                height
            )
        ),
        title
    ) {
    font.openFromFile(
        "C:/Windows/Fonts/arial.ttf"
    );
}

sf::Color Visualizer::getTrafficColor(
    const Edge& edge
) {
    if (edge.blocked) {
        return sf::Color(
            70,
            70,
            70
        );
    }

    int increase =
        edge.travelTime -
        edge.baseTravelTime;

    if (increase <= 0) {
        return sf::Color(
            180,
            180,
            180
        );
    }

    if (increase <= 5) {
        return sf::Color(
            255,
            255,
            0
        );
    }

    if (increase <= 10) {
        return sf::Color(
            255,
            165,
            0
        );
    }

    return sf::Color(
        180,
        0,
        180
    );
}

bool Visualizer::isRouteEdge(
    const vector<int>& path,
    int from,
    int to
) {
    for (
        int i = 0;
        i + 1 < static_cast<int>(path.size());
        i++
    ) {
        if (
            (path[i] == from &&
             path[i + 1] == to) ||
            (path[i] == to &&
             path[i + 1] == from)
        ) {
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
    return isRouteEdge(
        path,
        from,
        to
    );
}

int Visualizer::getClickedNode(
    const Graph& graph,
    sf::Vector2i mousePosition
) {
    const vector<Node>& nodes =
        graph.getNodes();

    for (
        int i = 0;
        i < static_cast<int>(nodes.size());
        i++
    ) {
        float screenX =
            static_cast<float>(
                nodes[i].x * 50 + 100
            );

        float screenY =
            static_cast<float>(
                nodes[i].y * 50 + 100
            );

        float dx =
            mousePosition.x -
            screenX;

        float dy =
            mousePosition.y -
            screenY;

        if (
            dx * dx +
            dy * dy <=
            12 * 12
        ) {
            return i;
        }
    }

    return -1;
}

pair<int, int> Visualizer::getClickedRoad(
    const Graph& graph,
    sf::Vector2i mousePosition
) {
    const vector<Node>& nodes =
        graph.getNodes();

    float mouseX =
        static_cast<float>(
            mousePosition.x
        );

    float mouseY =
        static_cast<float>(
            mousePosition.y
        );

    float bestDistance =
        12.0f;

    pair<int, int> selectedRoad =
        {-1, -1};

    for (
        int i = 0;
        i < static_cast<int>(nodes.size());
        i++
    ) {
        for (
            const auto& edge :
            nodes[i].edges
        ) {
            int j =
                edge.destination;

            if (i > j) {
                continue;
            }

            float x1 =
                static_cast<float>(
                    nodes[i].x * 50 + 100
                );

            float y1 =
                static_cast<float>(
                    nodes[i].y * 50 + 100
                );

            float x2 =
                static_cast<float>(
                    nodes[j].x * 50 + 100
                );

            float y2 =
                static_cast<float>(
                    nodes[j].y * 50 + 100
                );

            float dx =
                x2 - x1;

            float dy =
                y2 - y1;

            float lengthSquared =
                dx * dx +
                dy * dy;

            if (lengthSquared == 0) {
                continue;
            }

            float t =
                (
                    (mouseX - x1) * dx +
                    (mouseY - y1) * dy
                ) /
                lengthSquared;

            t =
                max(
                    0.0f,
                    min(
                        1.0f,
                        t
                    )
                );

            float closestX =
                x1 + t * dx;

            float closestY =
                y1 + t * dy;

            float distanceX =
                mouseX - closestX;

            float distanceY =
                mouseY - closestY;

            float distance =
                sqrt(
                    distanceX * distanceX +
                    distanceY * distanceY
                );

            if (distance < bestDistance) {
                bestDistance =
                    distance;

                selectedRoad = {
                    i,
                    j
                };
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
    if (
        source == -1 ||
        destination == -1
    ) {
        return;
    }

    lpaStar.initialize(
        graph,
        source,
        destination
    );

    auto start =
        chrono::high_resolution_clock::now();

    lpaStar.computeShortestPath(
        graph
    );

    RouteResult result =
        lpaStar.getCurrentPath(
            graph
        );

    auto end =
        chrono::high_resolution_clock::now();

    double timeMs =
        chrono::duration<double, milli>(
            end - start
        ).count();

    route =
        result;

    previousLpaNodes =
        result.nodesExplored;

    lpaInitialized =
        true;

    printInitialRoute(
        route,
        timeMs
    );
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

    lpaStar.updateEdge(
        graph,
        from,
        to
    );

    lpaStar.computeShortestPath(
        graph
    );

    route =
        lpaStar.getCurrentPath(
            graph
        );
}

void Visualizer::run(
    Graph& graph
) {
    int source = -1;
    int destination = -1;

    int selectedFrom = -1;
    int selectedTo = -1;

    RouteResult route;

    while (window.isOpen()) {

        while (const optional event =
            window.pollEvent()) {

            if (
                event->is<
                    sf::Event::Closed
                >()
            ) {
                window.close();
            }

            if (
                const auto* mouseButton =
                    event->getIf<
                        sf::Event::MouseButtonPressed
                    >()
            ) {
                if (
                    mouseButton->button ==
                    sf::Mouse::Button::Left
                ) {
                    sf::Vector2i mousePosition(
                        mouseButton->position.x,
                        mouseButton->position.y
                    );

                    int clickedNode =
                        getClickedNode(
                            graph,
                            mousePosition
                        );

                    if (clickedNode != -1) {

                        if (source == -1) {

                            source =
                                clickedNode;

                            cout
                                << "Source selected: "
                                << source
                                << "\n";

                        } else if (
                            destination == -1 &&
                            clickedNode != source
                        ) {

                            destination =
                                clickedNode;

                            cout
                                << "Destination selected: "
                                << destination
                                << "\n";

                            calculateInitialRoute(
                                graph,
                                source,
                                destination,
                                route
                            );
                        }

                    } else if (
                        source != -1 &&
                        destination != -1
                    ) {

                        pair<int, int> road =
                            getClickedRoad(
                                graph,
                                mousePosition
                            );

                        if (road.first != -1) {

                            selectedFrom =
                                road.first;

                            selectedTo =
                                road.second;

                            cout
                                << "\nSelected road: "
                                << selectedFrom
                                << " <-> "
                                << selectedTo
                                << "\n";
                        }
                    }
                }
            }

            if (
                const auto* key =
                    event->getIf<
                        sf::Event::KeyPressed
                    >()
            ) {
                if (
                    selectedFrom == -1 ||
                    selectedTo == -1 ||
                    source == -1 ||
                    destination == -1
                ) {
                    continue;
                }

                if (
                    key->code ==
                    sf::Keyboard::Key::Up
                ) {
                    bool affected =
                        routeAffected(
                            route.path,
                            selectedFrom,
                            selectedTo
                        );

                    graph.increaseTraffic(
                        selectedFrom,
                        selectedTo
                    );

                    cout
                        << "\nTraffic increased on road "
                        << selectedFrom
                        << " <-> "
                        << selectedTo
                        << "\n";

                    if (!affected) {
                        cout
                            << "Hybrid LPA*: current route not affected. No repair needed.\n";

                        printNoRepair(
                            route
                        );

                        continue;
                    }

                    auto start =
                        chrono::high_resolution_clock::now();

                    repairRoute(
                        graph,
                        source,
                        destination,
                        selectedFrom,
                        selectedTo,
                        route
                    );

                    auto end =
                        chrono::high_resolution_clock::now();

                    double timeMs =
                        chrono::duration<double, milli>(
                            end - start
                        ).count();

                    int nodesProcessed =
                        max(
                            0,
                            route.nodesExplored -
                            previousLpaNodes
                        );

                    previousLpaNodes =
                        route.nodesExplored;

                    cout
                        << "Hybrid LPA*: incremental repair completed.\n";

                    printRepairResult(
                        route,
                        timeMs,
                        nodesProcessed
                    );
                }

                if (
                    key->code ==
                    sf::Keyboard::Key::Down
                ) {
                    graph.decreaseTraffic(
                        selectedFrom,
                        selectedTo
                    );

                    cout
                        << "\nTraffic decreased on road "
                        << selectedFrom
                        << " <-> "
                        << selectedTo
                        << "\n";

                    auto start =
                        chrono::high_resolution_clock::now();

                    repairRoute(
                        graph,
                        source,
                        destination,
                        selectedFrom,
                        selectedTo,
                        route
                    );

                    auto end =
                        chrono::high_resolution_clock::now();

                    double timeMs =
                        chrono::duration<double, milli>(
                            end - start
                        ).count();

                    int nodesProcessed =
                        max(
                            0,
                            route.nodesExplored -
                            previousLpaNodes
                        );

                    previousLpaNodes =
                        route.nodesExplored;

                    cout
                        << "Hybrid LPA*: incremental repair completed.\n";

                    printRepairResult(
                        route,
                        timeMs,
                        nodesProcessed
                    );
                }

                if (
                    key->code ==
                    sf::Keyboard::Key::C
                ) {
                    bool affected =
                        routeAffected(
                            route.path,
                            selectedFrom,
                            selectedTo
                        );

                    graph.closeRoad(
                        selectedFrom,
                        selectedTo
                    );

                    cout
                        << "\nRoad closed: "
                        << selectedFrom
                        << " <-> "
                        << selectedTo
                        << "\n";

                    if (!affected) {
                        cout
                            << "Hybrid LPA*: current route not affected. No repair needed.\n";

                        printNoRepair(
                            route
                        );

                        continue;
                    }

                    auto start =
                        chrono::high_resolution_clock::now();

                    repairRoute(
                        graph,
                        source,
                        destination,
                        selectedFrom,
                        selectedTo,
                        route
                    );

                    auto end =
                        chrono::high_resolution_clock::now();

                    double timeMs =
                        chrono::duration<double, milli>(
                            end - start
                        ).count();

                    int nodesProcessed =
                        max(
                            0,
                            route.nodesExplored -
                            previousLpaNodes
                        );

                    previousLpaNodes =
                        route.nodesExplored;

                    cout
                        << "Hybrid LPA*: incremental repair completed.\n";

                    printRepairResult(
                        route,
                        timeMs,
                        nodesProcessed
                    );
                }

                if (
                    key->code ==
                    sf::Keyboard::Key::O
                ) {
                    graph.openRoad(
                        selectedFrom,
                        selectedTo
                    );

                    cout
                        << "\nRoad opened: "
                        << selectedFrom
                        << " <-> "
                        << selectedTo
                        << "\n";

                    auto start =
                        chrono::high_resolution_clock::now();

                    repairRoute(
                        graph,
                        source,
                        destination,
                        selectedFrom,
                        selectedTo,
                        route
                    );

                    auto end =
                        chrono::high_resolution_clock::now();

                    double timeMs =
                        chrono::duration<double, milli>(
                            end - start
                        ).count();

                    int nodesProcessed =
                        max(
                            0,
                            route.nodesExplored -
                            previousLpaNodes
                        );

                    previousLpaNodes =
                        route.nodesExplored;

                    cout
                        << "Hybrid LPA*: incremental repair completed.\n";

                    printRepairResult(
                        route,
                        timeMs,
                        nodesProcessed
                    );
                }

                if (
                    key->code ==
                    sf::Keyboard::Key::R
                ) {
                    graph.resetTraffic(
                        selectedFrom,
                        selectedTo
                    );

                    cout
                        << "\nTraffic reset on road "
                        << selectedFrom
                        << " <-> "
                        << selectedTo
                        << "\n";

                    auto start =
                        chrono::high_resolution_clock::now();

                    repairRoute(
                        graph,
                        source,
                        destination,
                        selectedFrom,
                        selectedTo,
                        route
                    );

                    auto end =
                        chrono::high_resolution_clock::now();

                    double timeMs =
                        chrono::duration<double, milli>(
                            end - start
                        ).count();

                    int nodesProcessed =
                        max(
                            0,
                            route.nodesExplored -
                            previousLpaNodes
                        );

                    previousLpaNodes =
                        route.nodesExplored;

                    cout
                        << "Hybrid LPA*: incremental repair completed.\n";

                    printRepairResult(
                        route,
                        timeMs,
                        nodesProcessed
                    );
                }
            }
        }

        window.clear(
            sf::Color::Black
        );

        const vector<Node>& nodes =
            graph.getNodes();

        for (
            int i = 0;
            i < static_cast<int>(nodes.size());
            i++
        ) {
            float x1 =
                static_cast<float>(
                    nodes[i].x * 50 + 100
                );

            float y1 =
                static_cast<float>(
                    nodes[i].y * 50 + 100
                );

            for (
                const auto& edge :
                nodes[i].edges
            ) {
                int j =
                    edge.destination;

                if (i > j) {
                    continue;
                }

                float x2 =
                    static_cast<float>(
                        nodes[j].x * 50 + 100
                    );

                float y2 =
                    static_cast<float>(
                        nodes[j].y * 50 + 100
                    );

                sf::Color roadColor;

                if (
                    isRouteEdge(
                        route.path,
                        i,
                        j
                    )
                ) {
                    roadColor =
                        sf::Color::Red;

                } else if (
                    edge.blocked
                ) {
                    roadColor =
                        sf::Color(
                            70,
                            70,
                            70
                        );

                } else if (
                    selectedFrom == i &&
                    selectedTo == j
                ) {
                    roadColor =
                        sf::Color::Cyan;

                } else {
                    roadColor =
                        getTrafficColor(
                            edge
                        );
                }

                sf::Vertex line[2];

                line[0].position =
                    sf::Vector2f(
                        x1,
                        y1
                    );

                line[0].color =
                    roadColor;

                line[1].position =
                    sf::Vector2f(
                        x2,
                        y2
                    );

                line[1].color =
                    roadColor;

                window.draw(
                    line,
                    2,
                    sf::PrimitiveType::Lines
                );
            }
        }

        for (
            int i = 0;
            i < static_cast<int>(nodes.size());
            i++
        ) {
            float x =
                static_cast<float>(
                    nodes[i].x * 50 + 100
                );

            float y =
                static_cast<float>(
                    nodes[i].y * 50 + 100
                );

            sf::CircleShape circle(
                5.0f
            );

            circle.setPosition(
                sf::Vector2f(
                    x - 5,
                    y - 5
                )
            );

            if (i == source) {
                circle.setFillColor(
                    sf::Color::Green
                );

            } else if (
                i == destination
            ) {
                circle.setFillColor(
                    sf::Color::Blue
                );

            } else {
                circle.setFillColor(
                    sf::Color::White
                );
            }

            window.draw(
                circle
            );

            if (
                font.getInfo().family != ""
            ) {
                sf::Text text(
                    font,
                    to_string(i),
                    12
                );

                text.setFillColor(
                    sf::Color::White
                );

                text.setPosition(
                    sf::Vector2f(
                        x + 7,
                        y - 8
                    )
                );

                window.draw(
                    text
                );
            }
        }

        window.display();
    }
}

bool Visualizer::isOpen() {
    return window.isOpen();
}