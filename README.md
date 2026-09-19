<!--
Build:
cmake --build build

Run:
.\build\TrafficRouteEngine.exe

If CMake configuration changes:
Remove-Item -Recurse -Force build
cmake -S . -B build -G Ninja
cmake --build build

Benchmarks:

cmake --build build --target HybridBenchmark
.\build\HybridBenchmark.exe

Tests:
ctest --test-dir build --output-on-failure
-->

# 🚦 Traffic Route Engine

A scalable C++ traffic-aware routing engine for dynamic weighted road networks, supporting real-time traffic changes, road closures, automatic rerouting, alternative routes, incremental shortest-path repair, and large-scale performance benchmarking.

The engine combines **Dijkstra, A\*, Selective A\*, LPA\***, and a hybrid **Selective + LPA\*** routing strategy with an **SFML visualization layer**, **OpenStreetMap road networks**, automated testing, and a C++/Python benchmarking pipeline.

---

## 📌 Overview

Traffic Route Engine models a road network as a weighted graph and continuously adapts routes as traffic conditions and road availability change.

The system supports:

- Dynamic traffic updates
- Road closures and reopenings
- Automatic rerouting
- Selective rerouting
- Incremental shortest-path repair using LPA\*
- K-shortest alternative routes
- Real-world OpenStreetMap road networks
- Interactive SFML visualization
- Large-scale graphs with 1M+ nodes
- Automated GoogleTest/CTest validation
- C++ benchmarking with CSV export
- Python/pandas/Matplotlib performance analysis

The project compares traditional full shortest-path recomputation against approaches that avoid unnecessary work or reuse previously computed routing state.

---

## 🚀 Key Features

### Weighted Graph Engine

- Adjacency-list graph representation
- Node IDs with geographic or grid coordinates
- Weighted road edges
- Base travel time and dynamic travel time
- Traffic-aware edge costs
- Edge lookup and weight updates
- Blocked-road state
- Dynamic graph modifications

### Dynamic Traffic Simulation

- Increase traffic on roads
- Decrease traffic on roads
- Reset traffic
- Multiple traffic levels
- Dynamic edge-cost updates
- Traffic-aware route recalculation
- Visual congestion representation

### Road Closures

- Close individual roads
- Reopen roads
- Closed roads excluded from routing
- Automatic rerouting after relevant closures
- Reopened roads can become available for route selection

---

## 🧠 Routing Algorithms

### Dijkstra

Implemented from scratch using a priority queue.

Features:

- Distance tracking
- Parent tracking
- Path reconstruction
- Dynamic edge weights
- Blocked-road handling
- Nodes-explored measurement

### A\*

Implemented from scratch using coordinate-based heuristics.

Features:

- `g(n)` cost
- `h(n)` heuristic
- `f(n) = g(n) + h(n)`
- Coordinate-based heuristic
- Path reconstruction
- Dynamic traffic handling
- Blocked-road handling
- Nodes-explored measurement

### Selective A\*

Instead of recomputing the route after every traffic update, the engine checks whether the changed road affects the current route.

```text
Traffic Update
      |
      v
Does the changed edge affect
the current route?
      |
   +--+--+
   |     |
  Yes    No
   |     |
   v     v
Run A*  Skip
   |
   v
Update Route
```

This avoids unnecessary full A\* executions when traffic changes do not affect the active route.

Traffic decreases and road reopenings are also evaluated because an off-route road may become part of a better route.

### LPA\*

Lifelong Planning A\* is implemented for incremental shortest-path repair.

Instead of rebuilding the entire search state after every graph modification:

```text
Initial Search
      |
      v
Previous Search State
      |
      v
Graph Update
      |
      v
Update Affected State
      |
      v
Incremental Repair
      |
      v
Updated Shortest Path
```

The system measures:

- Incremental repair time
- Nodes processed during repair
- Search-state reuse
- Updated route cost
- Updated route

### Hybrid Selective + LPA\*

The hybrid strategy combines both optimizations.

```text
Traffic / Road Update
          |
          v
Is the affected road relevant
to the current route?
          |
      +---+---+
      |       |
     No      Yes
      |       |
      v       v
    Skip    LPA* Repair
              |
              v
        Updated Route
```

This allows unaffected updates to be skipped while affected updates are handled through incremental shortest-path repair.

---

## 🛣️ Alternative Routes

The routing engine supports generation and comparison of multiple valid routes.

Alternative routing supports:

- K-shortest routes
- Route cost comparison
- Multiple route candidates
- Route selection based on travel cost
- Dynamic route alternatives under changing traffic

---

## 🌍 OpenStreetMap Integration

The engine supports real-world road networks using OpenStreetMap data.

The OSM pipeline converts real road-network information into the internal routing graph:

```text
OpenStreetMap Data
        |
        v
Road / Intersection Data
        |
        v
Graph Construction
        |
        v
Weighted Road Network
        |
        v
Dijkstra / A* / LPA*
        |
        v
Route
```

Real geographic coordinates are used for routing and visualization.

---

## 🎮 SFML Visualization

The project includes an interactive SFML visualization layer.

### Visualization Features

- Road network rendering
- Nodes and edges
- Node IDs
- Source and destination selection
- Click-based routing
- Current route highlighting
- Live traffic updates
- Road closures
- Dynamic rerouting
- Alternative routes
- Traffic-level visualization

### Traffic Visualization

| Color | Meaning |
|---|---|
| Gray | Normal |
| Yellow | Low traffic |
| Orange | Medium traffic |
| Purple | Heavy traffic |
| Red | Current route |
| Cyan | Selected road |
| Dark gray | Closed road |

The visualization allows routing behavior to be observed as the network changes in real time.

---

## 📊 Performance Benchmarking

The project includes dedicated benchmark executables:

- `SelectiveAstar`
- `LPABenchmark`
- `HybridBenchmark`

The main benchmark compares:

- Naive A\*
- Selective A\*
- LPA\*
- Hybrid Selective + LPA\*

Each strategy is evaluated across multiple graph sizes and repeated trials.

### Benchmark Configuration

| Parameter | Value |
|---|---|
| Graph sizes | 100, 1,024, 2,500, and 10,000 nodes (10×10, 32×32, 50×50, 100×100 grids) |
| Traffic updates | 1,000 per trial |
| Trials | 5 per graph size |

Metrics include:

- Runtime
- Nodes explored
- Routing updates
- Skipped updates
- Time reduction
- Incremental repair work

---

## 📈 Benchmark Results

On a synthetic 10,000-node (100×100) grid graph with 1,000 dynamic traffic updates, averaged over 5 trials:

| Strategy | Runtime | Nodes Explored | Routing Updates | Time Reduction vs Naive |
|---|---:|---:|---:|---:|
| Naive A\* | 3,417,423.6 µs | 9,669,702.2 | 1,000 | — |
| Selective A\* | 31,948.2 µs | 84,915.0 | 8.8 | 99.07% |
| LPA\* | 978.0 µs | 602.4 | 1,000 | 99.97% |
| Hybrid | 2,177.0 µs | 222.2 | 8.4 | 99.94% |

### Key Results

**Selective A\***

- A\* executions reduced from 1,000 to 8.8
- 99.12% fewer A\* executions
- 99.07% computation-time reduction
- Approximately 107× speedup

**LPA\***

- 99.97% computation-time reduction
- Approximately 3,500× speedup compared with naive A\*

**Hybrid**

- Routing updates reduced from 1,000 to 8.4
- Explored approximately 43,500× fewer nodes than naive A\*
- 222.2 nodes vs 9.67M nodes

These results demonstrate the benefit of avoiding unnecessary full shortest-path recomputation and reusing previous routing state.

### Scaling Observations

- On the 10×10 grid, Selective A\* was faster than the Hybrid, so the simpler approach wins on very small graphs.
- From 32×32 upward, incremental methods dominate: LPA\* and the Hybrid are orders of magnitude faster than naive A\*.
- On the 100×100 grid, the Hybrid explores the fewest nodes, while LPA\* alone has the lowest wall-clock time. The Hybrid's route-membership checks and bookkeeping add overhead that node counts do not capture.
- Naive A\* cost grows sharply with graph size, while LPA\* update time stays around 1 ms.

---

## 📁 Benchmark Data Pipeline

Benchmark results are exported to:

```text
benchmark_results.csv
```

CSV fields include:

```text
graph_size
rows
cols
algorithm
runtime_us
nodes_explored
reroutes
skipped_updates
time_reduction_percent
```

The data is then analyzed using Python:

```text
C++ Benchmark
      |
      v
benchmark_results.csv
      |
      v
Python
      |
      +---- pandas
      |
      +---- Matplotlib
      |
      v
Performance Graphs
```

Generated visualizations include:

- Runtime comparison
- Nodes explored comparison
- Routing updates / repairs
- Time reduction

---

## 🧪 Automated Testing

The project includes a GoogleTest/CTest validation suite covering the major routing components.

Tests cover:

- Graph construction
- Edge operations
- Traffic updates
- Road closures
- Road reopening
- Dijkstra
- A\*
- LPA\*
- Route reconstruction
- Unreachable destinations
- Dynamic rerouting
- Selective rerouting behavior

Tests can be executed using:

```bash
ctest --test-dir build --output-on-failure
```

---

## 📈 Scalability

The routing engine is designed to operate on large weighted road networks and has been tested with graphs containing 1M+ nodes.

Large-scale evaluation focuses on:

- Runtime scaling
- Nodes explored
- Memory usage
- Rerouting frequency
- Incremental repair cost
- Performance under repeated traffic updates

This allows the routing strategies to be evaluated beyond small synthetic examples.

---

## 🏗️ Architecture

```text
                    +----------------------+
                    |   SFML Visualization |
                    |  User Input / Render |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |    Routing Engine    |
                    +----------+-----------+
                               |
              +----------------+----------------+
              |                |                |
              v                v                v
         +---------+      +---------+      +---------+
         | Dijkstra|      |   A*    |      |  LPA*   |
         +---------+      +---------+      +---------+
              |                |                |
              +----------------+----------------+
                               |
                               v
                    +----------------------+
                    | Selective Rerouting  |
                    |    + Hybrid LPA*     |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |     Graph Engine     |
                    |  Nodes / Edges /     |
                    |  Traffic / Closures  |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |  OpenStreetMap Data  |
                    +----------------------+

                               |
                               v

                    +----------------------+
                    |  Benchmark Framework |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |  CSV Benchmark Data  |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |  Python / pandas /   |
                    |  Matplotlib Analysis |
                    +----------------------+
```

---

## 🛠️ Tech Stack

**Core**

- C++17
- CMake
- Ninja
- MinGW-w64

**Routing**

- Dijkstra
- A\*
- Selective A\*
- LPA\*
- Hybrid Selective + LPA\*
- K-shortest alternatives

**Visualization**

- SFML 3.0.2
- OpenStreetMap

**Testing**

- GoogleTest
- CTest

**Benchmarking & Analysis**

- C++ benchmarking
- CSV
- Python
- pandas
- Matplotlib

---

## 📂 Project Structure

```text
TrafficRouteEngine/
│
├── include/
│   ├── AStar.h
│   ├── Dijkstra.h
│   ├── Edge.h
│   ├── Graph.h
│   ├── GraphGenerator.h
│   ├── LPAStar.h
│   └── Node.h
│
├── src/
│   ├── main.cpp
│   ├── AStar.cpp
│   ├── Dijkstra.cpp
│   ├── Edge.cpp
│   ├── Graph.cpp
│   ├── GraphGenerator.cpp
│   ├── LPAStar.cpp
│   └── Node.cpp
│
├── visualization/
│   └── Visualizer.cpp
│
├── tests/
│   ├── SelectiveAstar.cpp
│   ├── LPABenchmark.cpp
│   ├── HybridBenchmark.cpp
│   └── unit/
│
├── scripts/
│   └── plot_benchmark.py
│
├── benchmark_results.csv
├── benchmark_plots/
├── CMakeLists.txt
└── README.md
```

---

## ⚙️ Build

Configure the project:

```bash
cmake -S . -B build -G Ninja
```

Build:

```bash
cmake --build build
```

Run the visualizer:

```bash
.\build\TrafficRouteEngine.exe
```

---

## 🔬 Run Benchmarks

### Selective A\*

```bash
cmake --build build --target SelectiveAstar
.\build\SelectiveAstar.exe
```

### LPA\*

```bash
cmake --build build --target LPABenchmark
.\build\LPABenchmark.exe
```

### Hybrid

```bash
cmake --build build --target HybridBenchmark
.\build\HybridBenchmark.exe
```

The Hybrid benchmark generates:

```text
benchmark_results.csv
```

---

## 📊 Generate Performance Graphs

Install the Python dependencies:

```bash
pip install pandas matplotlib
```

Run:

```bash
python scripts/plot_benchmark.py
```

Generated graphs:

```text
benchmark_plots/
├── runtime_comparison.png
├── nodes_explored.png
├── reroutes.png
└── time_reduction.png
```

---

## 🎯 Project Goals

Traffic Route Engine was designed to explore how shortest-path algorithms behave when a road network is no longer static.

The project focuses on the transition from:

```text
Static Graph
     |
     v
Shortest Path
     |
     v
Dynamic Graph
     |
     v
Repeated Recalculation
     |
     v
Selective Rerouting
     |
     v
Incremental Shortest Path
```

The final system combines algorithmic routing, dynamic graph updates, incremental computation, real-world road data, interactive visualization, automated testing, and quantitative performance analysis into a single C++ routing engine.