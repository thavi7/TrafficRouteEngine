cmake_minimum_required(VERSION 3.20)

project(TrafficRouteEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(TrafficRouteEngine
    src/main.cpp
    src/Edge.cpp
    src/Node.cpp
    src/Graph.cpp
    src/Dijkstra.cpp
    src/AStar.cpp
    src/GraphGenerator.cpp
)

add_executable(RoutingBenchmark
    benchmarks/RoutingBenchmark.cpp
    src/Edge.cpp
    src/Node.cpp
    src/Graph.cpp
    src/Dijkstra.cpp
    src/AStar.cpp
    src/GraphGenerator.cpp
)