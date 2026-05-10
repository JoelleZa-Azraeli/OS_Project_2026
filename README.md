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

## Repository Link
[https://github.com/JoelleZa-Azraeli/OS_Project_2026](https://github.com/JoelleZa-Azraeli/OS_Project_2026)

[Watch the Project Demo Video Here](https://drive.google.com/file/d/1mPDqL7iRMkM3oXcwLWfEjXevz5LnptQP/view?usp=drive_link)

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
* `make clean`: Removes all compiled files.

### Execution
* **Milestone 1:** `./dijkstra <file_name>`
* **Milestones 2-3:** `./sim <file_name>`

## Implementation Overview
* **Milestone 1:** Core Dijkstra implementation using an Adjacency Matrix. It reads a graph and query from a text file, outputs the path with arrows, and handles edge cases such as unreachable nodes, source-equals-destination, and invalid negative weights.
* **Milestone 2:** Visualized the graph nodes, directed edges, and weights using **Raylib**.
* **Milestone 3:** Added a moving entity that follows the shortest path:
    * **Timing Logic:** 1-second pause at each intermediate node.
    * **Movement:** Segments edges into W steps, moving every 300ms.
    * **Controls:** Includes a Play/Stop button to control the animation.
