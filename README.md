# Pointer-Chasing – Microarchitecture Analysis

Benchmarking how memory layout affects CPU performance, by comparing pointer-chasing traversal over **linear** vs **randomized** memory allocations.

> **Status**: work in progress.

## Motivation

Modern CPUs rely heavily on caching and prefetching to hide memory latency. Predictable, contiguous memory access patterns let the hardware prefetcher do its job well; scattered, unpredictable access patterns defeat it and expose the real cost of cache misses and TLB misses.

This project builds a controlled experiment to make that cost measurable and visible, rather than just theoretical.

## How it works

### 1. The `Object` / `RessourceManager` model

Each `Object` is a node with a `next` pointer (used for traversal), plus `alt` and `meta` pointers (currently unused, reserved for future experiments, e.g. multiple traversal orders through the same dataset) and an `int val`. A `RessourceManager` holds an array of pointers (`handles`) to these nodes, plus the total count.

### 2. Two memory layouts

- **Linear layout** (`create_linear_rm`): all `Object`s are allocated one after another with plain `malloc`, then linked in allocation order (`handles[i]->next = handles[i+1]`). Physically contiguous, cache- and prefetcher-friendly.
- **Dispersed layout** (`create_dispersed_rm`): each `Object` is allocated separately with `posix_memalign(&obj, 4096, sizeof(Object))`, forcing every node onto its own memory page. The array of pointers is then shuffled with the **Fisher-Yates algorithm** (iterating backwards, swapping each element with a random earlier one), and nodes are linked (`next`) according to this shuffled order. The result: consecutive traversal steps jump to essentially random pages in memory, defeating spatial locality and the hardware prefetcher.

### 3. Pointer chasing (`traverse_next`)

Traversal starts at `handles[0]` and follows `->next` until `NULL`, summing `val` along the way. Each access depends on the result of the previous one (the address of the next node is only known after dereferencing the current one), so the CPU can't speculatively prefetch ahead  this isolates memory access latency as the dominant cost rather than raw compute or throughput.

### 4. Benchmarking

Both layouts are built for the same `N`, then traversed back-to-back, timed with `clock_gettime(CLOCK_MONOTONIC, ...)` (nanosecond resolution). `milestone_1.c` extends this to a sweep across `N = 10 000 / 100 000 / 1 000 000` and writes each run's timings to `results.csv`.

### 5. Analysis (`graph.py`)

Reads `results.csv` and produces two plots with `matplotlib`:
- **`graph_temps.png`** traversal time (ns) vs `N`, linear vs dispersed.
- **`graph_ratio.png`** ratio (dispersed time / linear time) vs `N`, to visualize how much the access pattern penalty grows with dataset size.


## Build & run

```bash
gcc -O2 -o pointer_chase main.c ressourceManager.c
./pointer_chase

# for the full sweep + CSV export:
gcc -O2 -o milestone1 milestone_1.c ressourceManager.c
./milestone1
python3 graph.py
```

## What this demonstrates

- Practical, hands-on exploration of cache/TLB effects, not just citing the theory, but building an experiment that makes the cost of a bad access pattern measurable.
- Low-level C: manual memory management (`malloc`/`posix_memalign`/`free`), pointer-based data structures, explicit control over memory alignment.
- Correct implementation of a non-trivial algorithm (Fisher-Yates) applied to a systems-programming problem, not just a textbook exercise.
- Designing a controlled experiment: same dataset, same traversal logic, same timer, the only variable that changes between the two runs is the memory layout.
- A full, if small, data pipeline: C benchmark → CSV → Python/matplotlib visualization.

## Known limitations

- `rand()` is used without an explicit `srand()` seed, so the shuffle is deterministic across runs (same sequence every time) rather than truly randomized per execution, fine for reproducibility, but worth calling out since it wasn't a deliberate design choice documented as such.
- Each `N` is measured with a **single run**, with no warm-up pass and no averaging over multiple repetitions, results can be noisy, especially from OS scheduling or other processes competing for cache/memory bandwidth during the run.
- Timing is wall-clock based (`clock_gettime`), not hardware performance counters (e.g. `perf stat -e cache-misses,dTLB-load-misses`), so the results show the *effect* (latency) but don't directly measure *why* (actual cache miss / TLB miss counts).
- The `alt` and `meta` fields in `Object` are currently unused, they inflate the struct size (and therefore the memory footprint per node) without serving a purpose yet.
