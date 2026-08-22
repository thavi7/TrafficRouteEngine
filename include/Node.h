#ifndef NODE_H
#define NODE_H

#include <vector>
#include "Edge.h"

using namespace std;

class Node {
public:
    int id;
    double x;
    double y;
    vector<Edge> edges;

    Node(int id, double x = 0, double y = 0);
};

#endif