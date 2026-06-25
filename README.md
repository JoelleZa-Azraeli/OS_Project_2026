# OS Project - Graph Traffic Simulation
**Team:** Bingo Logic (yes we have a team name for morale purposes)

## Partners Information
* **Partner 1:** Hiba Kljawe
  - ID Num: 326795010
  - GitHub: [HiBA-Kl](https://github.com/HiBA-Kl)
  - Emails: hibajone31@gmail.com / halakl@post.jce.ac.il
* **Partner 2:** Joelle Zanbil
  - ID Num: 215037862
  - GitHub: [JoelleZa-Azraeli](https://github.com/JoelleZa-Azraeli)
  - Emails: joellezanbil2911@gmail.com / joelleza@post.jce.ac.il
* **Partner 3:** Maysa Lahaleh
  - ID Num: 214003873
  - GitHub: [mayslah](https://github.com/mayslah)
  - Emails: mays.lahaleh@gmail.com / lahalehma@post.jce.ac.il

## Repository Link
[https://github.com/JoelleZa-Azraeli/OS_Project_2026](https://github.com/JoelleZa-Azraeli/OS_Project_2026)

[Watch the Project Demo Video Here](https://drive.google.com/file/d/1uzkI6mWPtOYlwx3lZ8J1jxnrC9x2IfGB/view?usp=drive_link)

## Project Environment
* **OS:** Linux (Ubuntu/Debian)
* **Compiler:** gcc
* **Graphics Library:** Raylib (for Milestones 2+)

## Technical Instructions

### Compilation
We provide a `Makefile` with the following targets:
* `make milestone1`: Builds the Dijkstra executable (`dijkstra`).
* `make milestone2`: Builds the GUI visualization executable (`sim`).
* `make milestone3`: Builds the animation simulation executable (`sim`).
* `make milestone4`: Builds the multi-process simulation (`sim`).
* `make milestone5`: Builds the pipe-based IPC simulation (`sim`).
* `make milestone6`: Builds the semaphore-synchronized simulation (`sim`).
* `make milestone7`: Builds the scheduling algorithm simulation (`sim`).
* `make clean`: Removes all compiled files.

### Execution
* **Milestone 1:** `./dijkstra <file_name>`
* **Milestones 2-3:** `./sim <file_name>`
* **Milestones 4-6:** `./sim <file_name>`
* **Milestone 7:** `./sim -schd fcfs <file_name>` or `./sim -schd sjf <file_name>`

## Implementation Overview
* **Milestone 1:** Core Dijkstra implementation using an Adjacency Matrix. It reads a graph and query from a text file, outputs the path with arrows, and handles edge cases such as unreachable nodes, source-equals-destination, and invalid negative weights.
* **Milestone 2:** Visualized the graph nodes, directed edges, and weights using **Raylib**.
* **Milestone 3:** Added a moving entity that follows the shortest path:
    * **Timing Logic:** 1-second pause at each intermediate node.
    * **Movement:** Segments edges into W steps, moving every 300ms.
    * **Controls:** Includes a Play/Stop button to control the animation.
* **Milestone 4:** Multiple travelers as separate child processes managed by the parent:
    * Parent forks one child per traveler; parent pre-computes all shortest paths.
    * Parent draws smooth animated balls gliding along edges using Raylib.
    * When a traveler finishes its path, parent sends `SIGTERM` to terminate the child.
    * Includes a Play/Stop button to start/pause all travelers.
* **Milestone 5:** Children communicate via anonymous pipes using non-blocking reads:
    * Each child computes its own Dijkstra path autonomously.
    * Children send `MSG_ARRIVED` and `MSG_FINISHED` messages through a pipe to the parent.
    * Parent reads pipes non-blocking each frame and updates the GUI accordingly.
    * Console logs each arrival and destination event with PID.
* **Milestone 6:** Node-access synchronization with POSIX semaphores in shared memory:
    * One semaphore per node (initialized to 1) in `mmap(MAP_SHARED|MAP_ANONYMOUS)` memory.
    * Children send `MSG_WAITING`, call `sem_wait`, then `MSG_ARRIVED`, then `sem_post`.
    * Waiting travelers are drawn in **orange** outside their target node.
    * Demonstrates mutual exclusion at bottleneck nodes (e.g. node 2 in `input_m6.txt`).
* **Milestone 7:** Parent-controlled scheduling replaces random semaphore acquisition:
    * Each traveler gets a **personal semaphore** (starts at 0 — blocks immediately).
    * Children send `M7_WAITING` → block; send `M7_LEAVING` after `sleep(1)`.
    * Parent maintains a per-node waiting queue and decides who enters next.
    * **FCFS (First Come First Served):** travelers enter nodes in the order they arrived.
    * **SJF (Shortest Job First):** the traveler with fewest remaining hops gets priority.
    * Active scheduler name shown in the top-left corner of the GUI.

### FCFS vs SJF — Algorithm Comparison

| | FCFS | SJF |
|---|---|---|
| **Selection rule** | First to arrive waits first | Fewest remaining hops goes first |
| **Fairness** | Fully fair — no starvation | Can starve long-path travelers |
| **Wait time** | Predictable, arrival-order | Shorter travelers finish sooner |
| **Best for** | Equal-length paths | Mixed path lengths |

With the `input_m6.txt` bottleneck graph (all 3 travelers queue at node 2):
* **FCFS** — whichever traveler sends `M7_WAITING` first enters node 2 first.
* **SJF** — the traveler with the fewest hops remaining (closest to destination) enters first, so travelers on shorter remaining paths complete faster at the cost of longer-path travelers waiting longer.
