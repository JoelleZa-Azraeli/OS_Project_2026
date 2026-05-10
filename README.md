# OS Project: Graph Traffic Simulation
**Team: Bingo Logic** | **Developers:** Hiba Kljawe & Joelle Zanbil

## 📌 Overview
A high-performance simulation of traffic flow on a weighted directed graph. This project implements Dijkstra's algorithm for pathfinding and uses the Raylib library for real-time visualization.

## 🛠 Technical Features
- **Data Architecture:** Implemented via a **Dynamic Adjacency List** for efficient edge traversal.
- **Robust Logic:** - Strict validation against **negative edge weights**.
  - Graceful handling of unreachable nodes ("No path found").
  - Memory-safe implementation with dedicated cleanup functions to prevent leaks.
- **GUI Visualization:** Dynamic circular node placement with directional, weight-labeled arrows.

## 🚀 How to Build and Run

### Prerequisites
Install `raylib` on your Linux/VM environment:
```bash
sudo apt update && sudo apt install libraylib-dev
