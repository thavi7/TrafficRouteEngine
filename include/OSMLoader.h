#ifndef OSM_LOADER_H
#define OSM_LOADER_H

#include <string>
#include "Graph.h"

class OSMLoader {
public:
    static Graph load(const std::string& filePath);
};

#endif