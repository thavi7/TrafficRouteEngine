#ifndef EDGE_H
#define EDGE_H

class Edge {
public:
    int destination;
    int travelTime;
    bool blocked;

    Edge(int destination, int travelTime);
};

#endif