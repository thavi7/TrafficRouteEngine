import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent
CSV_FILE = BASE_DIR / "benchmark_results.csv"
OUTPUT_DIR = BASE_DIR / "benchmark_plots"

OUTPUT_DIR.mkdir(exist_ok=True)

df = pd.read_csv(CSV_FILE)

df["graph_size"] = df["graph_size"].astype(int)
df["runtime_us"] = df["runtime_us"].astype(float)
df["nodes_explored"] = df["nodes_explored"].astype(float)
df["reroutes"] = df["reroutes"].astype(float)
df["skipped_updates"] = df["skipped_updates"].astype(float)
df["time_reduction_percent"] = df["time_reduction_percent"].astype(float)

algorithms = ["Naive A*", "Selective A*", "LPA*", "Hybrid"]

def plot_metric(column, ylabel, filename, title):
    plt.figure(figsize=(10, 6))

    for algorithm in algorithms:
        data = df[df["algorithm"] == algorithm]
        plt.plot(
            data["graph_size"],
            data[column],
            marker="o",
            label=algorithm
        )

    plt.xlabel("Graph Size (nodes)")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(OUTPUT_DIR / filename, dpi=300)
    plt.close()


plot_metric(
    "runtime_us",
    "Runtime (µs)",
    "runtime_comparison.png",
    "Runtime Comparison"
)

plot_metric(
    "nodes_explored",
    "Nodes Explored",
    "nodes_explored.png",
    "Nodes Explored Comparison"
)

plot_metric(
    "reroutes",
    "Reroutes / Repairs",
    "reroutes.png",
    "Reroutes / Repairs Comparison"
)

plot_metric(
    "time_reduction_percent",
    "Time Reduction (%)",
    "time_reduction.png",
    "Time Reduction Compared with Naive A*"
)

print("Plots generated successfully.")
print(f"Output directory: {OUTPUT_DIR}")