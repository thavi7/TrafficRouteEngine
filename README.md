  <!-- 
  cmake --build build
  .\build\TrafficRouteEngine.exe

  change cmake

  cmake -S . -B build
  test

  cmake --build build --target SelectiveAstar
  .\build\SelectiveAstar.exe

  cmake --build build --target LPABenchmark
  .\build\LPABenchmark.exe 
  
  cmake --build build --target HybridBenchmark
  .\build\HybridBenchmark.exe
  -->

# 🚦 Traffic Route Engine

A C++ traffic-aware routing engine that simulates real-time congestion, road closures, and dynamic rerouting on a weighted graph — with a live SFML visualization and benchmarked performance comparisons between **Dijkstra** and **A\***.

---

## 📌 Overview

Traffic Route Engine models a city road network as a weighted graph and computes optimal routes under changing traffic conditions. It supports live traffic simulation, road closures, automatic rerouting, and a custom **selective rerouting optimization** that skips unnecessary pathfinding recalculations — measured at a **~38.9× speedup** over the naive approach on a 10,000-node graph.

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

### Traffic Simulation
- Increase / decrease / reset traffic per road
- Live traffic changes propagate into route cost calculations

### Road Closures
- `closeRoad()` / `openRoad()`
- Closed roads excluded from routing
- Automatic rerouting triggered on relevant closures/openings

### Pathfinding Algorithms
**Dijkstra** — implemented from scratch
- Priority queue, distance tracking, parent tracking
- Path reconstruction, blocked-road handling
- Dynamic-weight handling, nodes-explored measurement

**A\*** — implemented from scratch
- `g(n)`, `h(n)`, `f(n)` with coordinate-based heuristic
- Path reconstruction, blocked-road handling
- Nodes-explored measurement

**Dijkstra vs A\*** — terminal comparison of path, total travel time, and nodes explored. Verified A\* consistently explores fewer nodes on test graphs.

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
  - Dark = closed road

### Interactive Routing
- Select source → select destination → route auto-calculated
- Live updates on traffic change, road closure, and road reopening
- Automatic rerouting when relevant changes occur

### Selective Rerouting Optimization
- Previously: every traffic update triggered a full Dijkstra + A\* run
- Now: `isRouteEdge()` checks whether an update actually affects the current route
- Irrelevant updates are skipped entirely; only relevant changes trigger recalculation

### Benchmarking
Separate `RoutingBenchmark` executable, tested on a 10,000-node graph with 1,000 traffic updates:

| Strategy  | Dijkstra Runs | A\* Runs | Updates Skipped | Total Time |
|-----------|---------------|----------|------------------|------------|
| Naive     | 1,000         | 1,000    | 0                | 6,147,611 µs |
| Selective | 24            | 24       | 976              | 158,159 µs |

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

### Traffic-Level System
- Explicit traffic levels (increase / decrease / reset)
- Improved traffic visualization

### GoogleTest Suite
- Graph tests
- Dijkstra tests
- A\* tests
- Traffic update tests
- Road closure/opening tests
- Unreachable destination tests
- Rerouting tests

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

### Advanced Optimization
- Incremental shortest paths
- LPA* (Lifelong Planning A\*)
- D\* Lite
- Route caching
- Comparison against current selective rerouting

### Final Polish
- README, architecture documentation, complexity analysis
- Benchmark graphs, test coverage report
- Performance results write-up
- GitHub cleanup
- Resume-ready project description

---

## 🛠️ Tech Stack

- **Language:** C++17
- **Build:** CMake + Ninja
- **Compiler:** MinGW-w64
- **Visualization:** SFML
- **Planned:** GoogleTest, Python + Matplotlib, OSM data pipeline

---

## 📊 Architecture (High Level)

```
+-------------------+       +----------------------+       +--------------------+
|    Graph Engine    | <---> |  Routing Algorithms   | <---> |   SFML Frontend    |
| (nodes, edges,      |       | (Dijkstra, A*,        |       | (render, input,    |
|  traffic, closures) |       |  selective rerouting) |       |  visualization)    |
+-------------------+       +----------------------+       +--------------------+
                                      |
                                      v
                            +-------------------+
                            | Benchmark Suite    |
                            | (naive vs selective)|
                            +-------------------+
```

---

## 📄 License

TBD

## 🙌 Author

Avi