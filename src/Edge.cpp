#include "Edge.h"

Edge::Edge(int destination, int travelTime)
    : destination(destination),
      travelTime(travelTime),
      baseTravelTime(travelTime),
      blocked(false) {
}