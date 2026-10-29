import matplotlib.pyplot as plt

# ---------- read results.csv ----------
sizes = []        # working-set sizes in bytes
size_times = []   # ns per access for each working-set size
strides = []      # strides in bytes
stride_times = [] # ns per access for each stride


#using only a with method broke
import os

folder = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(folder, "results.csv")) as f:
    next(f)   # skip the header line
    for line in f:
        parts = line.strip().split(",")
        if len(parts) != 3:
            continue   # skip blank lines
        if parts[0] == "working_set":
            sizes.append(int(parts[1]))
            size_times.append(float(parts[2]))
        elif parts[0] == "stride":
            strides.append(int(parts[1]))
            stride_times.append(float(parts[2]))



# ------- graph 1: working-set size (cache sizes) ----------


plt.figure(figsize=(12, 6))
plt.plot(sizes, size_times, marker="o")
plt.xscale("log", base=2)
plt.yscale("log")
plt.xlabel("Working set size (bytes)")
plt.ylabel("Time per access (ns)")
plt.title("Time per access vs working set size")



# boundaries from /sys (cache sizes in bytes)
sys_bounds = {"L1 (32K)": 32 * 1024, "L2 (512K)": 512 * 1024, "L3 (4M)": 4096 * 1024}

for label, bound in sys_bounds.items():
    plt.axvline(bound, color="red", linestyle="-", linewidth=1.5)
    plt.text(bound, plt.ylim()[1], f" {label}", color="red", rotation=90,
             va="top", ha="right", fontsize=9)



plt.legend(handles=[
    plt.Line2D([0], [0], color="red", label="sys cache sizes"),
])


# -------- graph 2: stride (line size) ----------
plt.figure(figsize=(12, 6))
plt.plot(strides, stride_times, marker="o")
plt.xscale("log", base=2)
plt.xlabel("Stride (bytes)")
plt.ylabel("Time per access (ns)")
plt.title("Time per access vs stride")



# real cache line size from /sys
plt.axvline(64, color="red", linestyle="-", linewidth=1.5)
plt.legend(handles=[
    plt.Line2D([0], [0], color="red", label="sys line size (64 B)"),
])


plt.show()