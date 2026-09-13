# API-project-Movhex-Polimi

Final project for the **Algorithms and Data Structures** module of the
Algorithms and Principles of Computer Science course at **Politecnico di Milano**,
A.Y. 2024/2025.

**Final grade: 24/30**

MovHex is a command-line program written in **C** that computes minimum-cost
routes on a dynamic hexagonal map.

The project was evaluated through automated tests considering both correctness
and computational efficiency.

## Project overview

The map is represented as a rectangular grid of hexagonal cells.

Each cell has a traversal cost and may also have up to five directed air routes
to other cells. Both terrain costs and air-route costs can change during the
execution of the program.

The program supports four commands:

- `init` — initializes a new hexagonal map
- `change_cost` — modifies traversal costs within a given radius
- `toggle_air_route` — adds or removes a directed air connection
- `travel_cost` — computes the minimum travel cost between two cells

Input is read from `stdin` and results are written to `stdout`.

## Data structures

The implementation uses:

- a dynamically allocated two-dimensional map of hexagonal cells
- linked lists for outgoing air routes
- a linked queue for breadth-first traversal
- a custom binary min-heap for shortest-path computation
- per-cell metadata for distance, heap position and search versioning

The six terrestrial neighbors of each cell are determined from its position in
the hexagonal grid and are therefore not stored as explicit graph edges.

## Algorithms

### Cost updates

The `change_cost` operation uses a **breadth-first search (BFS)** starting from
the selected cell.

The traversal explores the portion of the hexagonal map within the requested
radius and updates both cell traversal costs and the costs of outgoing air
routes.

### Shortest path

The `travel_cost` operation uses **Dijkstra's algorithm** together with a custom
binary min-heap.

The graph combines the implicit connections between neighboring hexagonal cells
with the explicitly stored directed air routes.

A versioning mechanism is used to avoid resetting pathfinding metadata across
the entire map before every new search.

## Build

The project can be compiled with:

```bash
make
```

or directly with GCC:

```bash
gcc -Wall -Werror -std=gnu11 -O2 src/movhex.c -o movhex -lm
```

## Run

Run the program interactively with:

```bash
./movhex
```

or provide commands through a file:

```bash
./movhex < input.txt
```

## Technologies and concepts

C · Dynamic memory · Graph algorithms · Dijkstra · Binary heap · BFS ·
Linked lists · Queues · Hexagonal grids
