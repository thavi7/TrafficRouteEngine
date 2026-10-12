
#include "OSMLoader.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {

struct GeoNode {
    double lat;
    double lon;
};

struct RoadWay {
    vector<string> nodes;
    string highway;
};

map<string, string> getAttributes(const string& text) {
    map<string, string> attributes;
    size_t i = 0;

    while (i < text.size()) {
        while (i < text.size() &&
               (isspace(static_cast<unsigned char>(text[i])) ||
                text[i] == '/' || text[i] == '<' || text[i] == '>')) {
            ++i;
        }

        size_t start = i;

        while (i < text.size() &&
               !isspace(static_cast<unsigned char>(text[i])) &&
               text[i] != '=' && text[i] != '/' && text[i] != '>') {
            ++i;
        }

        if (start == i) {
            ++i;
            continue;
        }

        string key = text.substr(start, i - start);

        while (i < text.size() &&
               isspace(static_cast<unsigned char>(text[i]))) {
            ++i;
        }

        if (i >= text.size() || text[i] != '=') {
            continue;
        }

        ++i;

        while (i < text.size() &&
               isspace(static_cast<unsigned char>(text[i]))) {
            ++i;
        }

        if (i >= text.size() ||
            (text[i] != '"' && text[i] != '\'')) {
            continue;
        }

        char quote = text[i++];
        start = i;

        while (i < text.size() && text[i] != quote) {
            ++i;
        }

        attributes[key] = text.substr(start, i - start);

        if (i < text.size()) {
            ++i;
        }
    }

    return attributes;
}

bool isRoad(const string& highway) {
    static const set<string> excluded = {
        "footway", "cycleway", "path", "steps",
        "pedestrian", "bridleway", "corridor",
        "platform", "construction", "proposed"
    };

    return !highway.empty() && !excluded.count(highway);
}

double speedForRoad(const string& highway) {
    if (highway == "motorway" || highway == "motorway_link") return 25.0;
    if (highway == "trunk" || highway == "trunk_link") return 20.0;
    if (highway == "primary" || highway == "primary_link") return 16.0;
    if (highway == "secondary" || highway == "secondary_link") return 13.0;
    if (highway == "tertiary" || highway == "tertiary_link") return 11.0;
    if (highway == "residential" || highway == "living_street") return 8.0;
    if (highway == "service") return 5.0;

    return 9.0;
}

double distanceInMeters(const GeoNode& a, const GeoNode& b) {
    const double pi = acos(-1.0);
    const double radius = 6371000.0;

    double lat1 = a.lat * pi / 180.0;
    double lat2 = b.lat * pi / 180.0;
    double dLat = (b.lat - a.lat) * pi / 180.0;
    double dLon = (b.lon - a.lon) * pi / 180.0;

    double h = sin(dLat / 2.0) * sin(dLat / 2.0)
        + cos(lat1) * cos(lat2)
        * sin(dLon / 2.0) * sin(dLon / 2.0);

    h = max(0.0, min(1.0, h));

    return 2.0 * radius * asin(sqrt(h));
}

}

Graph OSMLoader::load(const string& filePath) {
    ifstream file(filePath, ios::binary);

    if (!file.is_open()) {
        throw runtime_error("Could not open OSM file: " + filePath);
    }

    string xml((istreambuf_iterator<char>(file)),
               istreambuf_iterator<char>());

    unordered_map<string, GeoNode> allNodes;
    vector<RoadWay> roads;

    RoadWay currentRoad;
    bool insideWay = false;

    size_t pos = 0;

    while ((pos = xml.find('<', pos)) != string::npos) {
        size_t end = xml.find('>', pos);

        if (end == string::npos) {
            break;
        }

        string tag = xml.substr(pos + 1, end - pos - 1);
        pos = end + 1;

        if (tag.empty() || tag[0] == '?' || tag[0] == '!') {
            continue;
        }

        bool closing = tag[0] == '/';

        if (closing) {
            tag.erase(0, 1);
        }

        size_t nameEnd = tag.find_first_of(" \t\r\n/");
        string name = tag.substr(0, nameEnd);

        if (name == "node" && !closing) {
            auto attributes = getAttributes(tag);

            if (attributes.count("id") &&
                attributes.count("lat") &&
                attributes.count("lon")) {
                allNodes[attributes["id"]] = {
                    stod(attributes["lat"]),
                    stod(attributes["lon"])
                };
            }
        } else if (name == "way" && !closing) {
            insideWay = true;
            currentRoad = RoadWay();
        } else if (name == "way" && closing) {
            if (insideWay && isRoad(currentRoad.highway) &&
                currentRoad.nodes.size() >= 2) {
                roads.push_back(move(currentRoad));
            }

            insideWay = false;
        } else if (insideWay && name == "nd" && !closing) {
            auto attributes = getAttributes(tag);

            if (attributes.count("ref")) {
                currentRoad.nodes.push_back(attributes["ref"]);
            }
        } else if (insideWay && name == "tag" && !closing) {
            auto attributes = getAttributes(tag);

            if (attributes.count("k") &&
                attributes["k"] == "highway" &&
                attributes.count("v")) {
                currentRoad.highway = attributes["v"];
            }
        }
    }

    set<string> usedNodeIds;

    for (const RoadWay& road : roads) {
        for (const string& id : road.nodes) {
            if (allNodes.count(id)) {
                usedNodeIds.insert(id);
            }
        }
    }

    if (usedNodeIds.empty()) {
        throw runtime_error("No usable road nodes found in OSM file.");
    }

    unordered_map<string, int> nodeIndex;
    vector<string> orderedIds;

    for (const string& id : usedNodeIds) {
        nodeIndex[id] = static_cast<int>(orderedIds.size());
        orderedIds.push_back(id);
    }

    double minLat = 90.0, maxLat = -90.0;
    double minLon = 180.0, maxLon = -180.0;

    for (const string& id : orderedIds) {
        const GeoNode& node = allNodes.at(id);

        minLat = min(minLat, node.lat);
        maxLat = max(maxLat, node.lat);
        minLon = min(minLon, node.lon);
        maxLon = max(maxLon, node.lon);
    }

    Graph graph(static_cast<int>(orderedIds.size()));

    for (int i = 0; i < static_cast<int>(orderedIds.size()); ++i) {
        const GeoNode& node = allNodes.at(orderedIds[i]);

        double x = maxLon == minLon
            ? 6.0
            : (node.lon - minLon) / (maxLon - minLon) * 12.0;

        double y = maxLat == minLat
            ? 4.0
            : (maxLat - node.lat) / (maxLat - minLat) * 8.0;

        graph.setNodePosition(i, x, y);
    }

    set<string> addedSegments;

    for (const RoadWay& road : roads) {
        for (int i = 1; i < static_cast<int>(road.nodes.size()); ++i) {
            const string& fromId = road.nodes[i - 1];
            const string& toId = road.nodes[i];

            if (!nodeIndex.count(fromId) || !nodeIndex.count(toId)) {
                continue;
            }

            int from = nodeIndex.at(fromId);
            int to = nodeIndex.at(toId);

            if (from == to) {
                continue;
            }

            int first = min(from, to);
            int second = max(from, to);
            string key = to_string(first) + ":" + to_string(second);

            if (!addedSegments.insert(key).second) {
                continue;
            }

            double meters = distanceInMeters(
                allNodes.at(fromId),
                allNodes.at(toId)
            );

            int travelTime = max(
                1,
                static_cast<int>(round(
                    meters / speedForRoad(road.highway)
                ))
            );

            graph.addEdge(from, to, travelTime);
            graph.addEdge(to, from, travelTime);
        }
    }

    cout << "Parsed OSM nodes: " << allNodes.size() << '\n';
    cout << "Road ways: " << roads.size() << '\n';
    cout << "Graph nodes: " << graph.getNodes().size() << '\n';

    return graph;
}
