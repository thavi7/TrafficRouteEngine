cmake_minimum_required(VERSION 3.20)

project(TrafficRouteEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(TrafficRouteEngine
    src/main.cpp
    src/Edge.cpp
    src/Node.cpp
    src/Graph.cpp
    src/Dijkstra.cpp
    src/AStar.cpp
    src/GraphGenerator.cpp
)



target_include_directories(TrafficRouteEngine PRIVATE
    include
)