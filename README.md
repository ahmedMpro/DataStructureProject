# Neon Cycle Explorer (Qt GUI)

Neon Cycle Explorer now ships as a Qt Widgets application that reuses the existing `Graph` and `DisjointSet`
cycle-detection implementations. The neon canvas exposes the full flow you requested: draw nodes based
on a vertex count, drag them freely, draw edges (directed or undirected), then run the cycle detector directly
from the GUI.

## Features

- Neon/dark “Neon Cycle Explorer” window built with Qt6 Widgets.
- Vertex count spinner + **Draw Graph** button to place glowing, draggable nodes arranged in a circle.
- Click once per node to draw an edge (loops allowed) while the radio buttons toggle between “Directed” and
   “Undirected” layout (the edges redraw with arrows when directed mode is selected).
- **Check Cyclic** runs the same logic already in `Graph::detectCycle()` (DFS for directed, union-find for
   undirected) and the result banner turns green or warning pink.
- Dragging nodes keeps edges connected, and duplicate edges are prevented.
- `src/console_demo.cpp` still exists if you want the old console-based walkthrough.

## Setup / Dependencies

1. Install a Qt 6 toolchain (MSVC or MinGW). Ensure the Qt `lib/cmake` directory is accessible via `CMAKE_PREFIX_PATH`.
2. Install CMake 3.16+ and a matching C++ compiler (MSVC 2019/2022 recommended for Windows).

## Build & Run (PowerShell example)

```powershell
cd d:\DataStructureProject
mkdir build
cd build
cmake -S .. -B . -DCMAKE_PREFIX_PATH="C:/Qt/6.7.1/msvc2019_64/lib/cmake"
cmake --build .
.\NeonCycleExplorer.exe
```

*Adjust the `CMAKE_PREFIX_PATH` to match your Qt installation (e.g., `C:/Qt/6.6.2/msvc2019_64/lib/cmake`). The
`cmake` call configures a Qt6 Widgets executable that links to `Qt6::Widgets`.*

## What to expect when running

1. The window opens with a dark neon background and the controls at the top.
2. Change the vertex spinner, tap **Draw Graph**, then click nodes to draw edges.
3. Toggle the Directed/Undirected radios to switch cycle detection semantics; the edges update visually.
4. Press **Check Cyclic** to see if the current adjacency graph contains a cycle; the result box turns green or pink.
5. Move nodes around to change layouts; edges stay attached automatically and cycle checks respect the new layout.

Use `console_demo.cpp` in `src` if you ever want a quick command-line verification of the cycle detection outputs without launching the GUI.
