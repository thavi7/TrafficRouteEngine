<!-- 
  cmake --build build
  .\build\TrafficRouteEngine.exe

  change cmake

  Remove-Item -Recurse -Force build
  cmake -S . -B build -G Ninja
  cmake --build build

  test

  cmake --build build --target SelectiveAstar
  .\build\SelectiveAstar.exe

  cmake --build build --target LPABenchmark
  .\build\LPABenchmark.exe 
  
  cmake --build build --target HybridBenchmark
  .\build\HybridBenchmark.exe
  -->

# 🚦 Traffic Route Engine

A C++ traffic-aware routing engine that simulates real-time congestion, road closures, and dynamic rerouting on a weighted graph — with a live SFML visualization and performance comparisons between **Dijkstra, A\*, Selective A\***, and **Hybrid LPA\***.

---

## 📌 Overview

Traffic Route Engine models a city road network as a weighted graph and computes optimal routes under changing traffic conditions. It supports live traffic simulation, road closures, automatic rerouting, selective rerouting, and incremental shortest-path repair using **LPA\***.

The project compares full shortest-path recomputation against optimized rerouting strategies that either skip unnecessary updates or reuse previous shortest-path computation state.

---

## ✅ Completed

### Project Setup
- C++17, CMake, Ninja, MinGW-w64
- Toolchain kept separate from competitive-programming setup
- SFML integrated for visualization

### Graph Engine
- Weighted graph with adjacency-list representation
- Node IDs with `(x, y)` coordinates
- Edge insertion, lookup, and weight updates
- `baseTravelTime` + dynamic `travelTime` per road
- Traffic-aware edge cost model
- Blocked-road state

### Traffic Simulation
- Increase / decrease / reset traffic per road
- Explicit traffic-level system
- Live traffic changes propagate into route cost calculations
- Traffic visualization based on congestion level

### Road Closures
- `closeRoad()` / `openRoad()`
- Closed roads excluded from routing
- Automatic rerouting triggered on relevant closures
- Reopened roads can introduce a better route

### Pathfinding Algorithms
**Dijkstra** — implemented from scratch
- Priority queue, distance tracking, parent tracking
- Path reconstruction, blocked-road handling
- Dynamic-weight handling
- Nodes-explored measurement

**A\*** — implemented from scratch
- `g(n)`, `h(n)`, `f(n)` with coordinate-based heuristic
- Path reconstruction, blocked-road handling
- Dynamic-weight handling
- Nodes-explored measurement

**Dijkstra vs A\*** — terminal comparison of runtime, travel time, and nodes explored.

### SFML Visualization
- Graph, roads, nodes, and node IDs rendered
- Click-to-select source and destination
- Click-to-select roads
- Traffic color coding:
  - Gray = normal
  - Yellow = low traffic
  - Orange = medium traffic
  - Purple = heavy traffic
  - Red = current route
  - Cyan = selected road
  - Dark Gray = closed road

### Interactive Routing
- Select source → select destination → route auto-calculated
- Live updates on traffic change, road closure, and road reopening
- Automatic route updates when the shortest path changes
- Hybrid LPA\* route displayed as the active route

### Selective Rerouting Optimization
- Previously: every traffic update triggered a full pathfinding recalculation
- Now: `isRouteEdge()` checks whether an update affects the current route
- Irrelevant traffic increases and road closures can be skipped
- Relevant changes trigger A\* recalculation
- Traffic decreases and road openings are evaluated because an off-route road can become a better route
- Skipped updates avoid unnecessary A\* executions

### LPA\* / Incremental Routing
- LPA\* implemented for incremental shortest-path computation
- Previous search state is reused after graph updates
- Incremental repair performed instead of rebuilding the complete search state
- Current route is updated after affected changes
- Repair time is measured separately
- Repair nodes processed are measured separately from previously processed nodes
- Unaffected updates can skip LPA\* repair

### Hybrid LPA\*
- Hybrid routing system integrated into the SFML visualizer
- Initial route computed using LPA\*
- Dynamic traffic and road changes trigger incremental repair
- Previous search state is reused
- Active route is generated from Hybrid LPA\*
- Console reports:
  - Repair time
  - Repair nodes processed
  - Previous search state reuse
  - Final route
  - Final travel time

### Benchmarking
Separate benchmark executables for evaluating routing and rerouting performance:

- `SelectiveAstar`
- `LPABenchmark`
- `HybridBenchmark`

Selective rerouting was tested on a 10,000-node graph with 1,000 traffic updates:

| Strategy  | Dijkstra Runs | A\* Runs | Updates Skipped | Total Time |
|-----------|---------------|----------|-----------------|------------|
| Naive     | 1,000         | 1,000    | 0               | 6,147,611 µs |
| Selective | 24            | 24       | 976             | 158,159 µs |

- **Time reduction:** 97.4273%
- **A\* execution reduction:** 97.6%
- **~38.9× faster** for this workload

---

## 🚧 Remaining Work

### Benchmark CSV
- Export benchmark results to CSV
- Store graph size, runtime, algorithm runs, skipped updates

### Python + Matplotlib
- Runtime comparison (naive vs selective)
- A\* executions vs graph size
- Time reduction vs graph size
- LPA\* repair performance visualization

### GoogleTest Suite
- Graph tests
- Dijkstra tests
- A\* tests
- LPA\* tests
- Traffic update tests
- Road closure/opening tests
- Unreachable destination tests
- Rerouting tests
- Selective rerouting tests

### Alternative Routes
- Generate multiple valid routes
- K-shortest / alternative path algorithm
- Route cost comparison

### OpenStreetMap Integration 🌍
- Obtain and parse real road data (OSM)
- Convert roads/intersections into the internal graph format
- Use real geographic coordinates
- Run Dijkstra/A\* on real-world road networks

### Real-World SFML Visualization
- Display real road network with source/destination, traffic, closed roads, current route, and rerouting

### Large-Scale Benchmarking
- Graph sizes: 10K, 50K, 100K, 500K+, 1M+ (where practical)
- Metrics: runtime, memory, nodes explored, rerouting time

### Realistic Traffic Experiments
- Random updates
- Updates on / near / far from the current route
- Repeated congestion
- Road closures/openings under load

### Final Polish
- README, architecture documentation, complexity analysis
- Benchmark graphs
- Test coverage report
- Performance results write-up
- GitHub cleanup
- Resume-ready project description

---

## 🛠️ Tech Stack

- **Language:** C++17
- **Build:** CMake + Ninja
- **Compiler:** MinGW-w64
- **Visualization:** SFML
- **Algorithms:** Dijkstra, A\*, Selective A\*, LPA\*
- **Planned:** GoogleTest, Python + Matplotlib, OSM data pipeline

---

## 📊 Architecture (High Level)

```text
+-------------------+       +----------------------+       +--------------------+
|    Graph Engine    | <---> |  Routing Algorithms   | <---> |   SFML Frontend    |
| (nodes, edges,     |       | (Dijkstra, A*,        |       | (render, input,    |
|  traffic, closures)|       |  selective, LPA*)     |       |  visualization)    |
+-------------------+       +----------------------+       +--------------------+
                                      |
                                      v
                            +-------------------+
                            | Benchmark Suite    |
                            | (naive vs selective|
                            |  vs incremental)   |
                            +-------------------+