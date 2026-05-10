# ===============================================================
# Project: Operating Systems Simulation - Traffic on Graph
# Team: Bingo Logic
# Developers: Hiba Kljawe & Joelle Zanbil
# ===============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Pointing to the Raylib folder and local src
INC = -Isrc -I$(HOME)/raylib/include
LDFLAGS = -L$(HOME)/raylib/lib
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Common logic files used by both versions
# We include dijkstra.c here so it's compiled into the final 'sim'
CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c

# Milestone 1: CLI Dijkstra (Text Output)
# Usage: make milestone1 -> ./dijkstra <input_file>
milestone1:
	$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) $(LDFLAGS) -o dijkstra $(LIBS)

# Milestone 2 & 3: GUI and Animation Simulation
# Usage: make milestone3 -> ./sim <input_file>
milestone2 milestone3:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# Cleanup: Removes executables and temporary object files
clean:
	rm -f dijkstra sim *.o

.PHONY: milestone1 milestone2 milestone3 clean
