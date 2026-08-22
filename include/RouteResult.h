#ifndef ROUTE_RESULT_H
#define ROUTE_RESULT_H

#include <vector>

using namespace std;

struct RouteResult {
    vector<int> path;
    int totalTime;
    int nodesExplored;
};

#endif