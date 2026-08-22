#include "Edge.h"

Edge::Edge(int destination, int travelTime) {
    this->destination = destination;
    this->travelTime = travelTime;
    this->blocked = false;
}