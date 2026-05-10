# ===============================================================
# Project: Operating Systems Simulation - Traffic on Graph
# Team: Bingo Logic
# Developers: Hiba Kljawe & Joelle Zanbil
# ===============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
# Raylib libraries for the GUI (Standard for Linux/Ubuntu)
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Common logic files used by both CLI and GUI versions
CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c
INC = -Isrc

# Milestone 1: CLI Dijkstra
milestone1:
	$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) -o dijkstra

# Milestone 2: Basic GUI Visualization
milestone2:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) -o sim $(LIBS)

# Milestone 3: Animation Simulation
# Uses -DANIMATION to trigger specific code blocks inside main_GUI.c
milestone3:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) -DANIMATION -o sim $(LIBS)

# Cleanup: Removes compiled files
clean:
	rm -f dijkstra sim *.o
