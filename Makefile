# ===============================================================
# Project: Operating Systems Simulation - Traffic on Graph
# Team: Bingo Logic
# Developers: Hiba Kljawe & Joelle Zanbil
# ===============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Pointing to the Raylib folder in your home directory
INC = -Isrc -I$(HOME)/raylib/include
LDFLAGS = -L$(HOME)/raylib/lib
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Common logic files used by both CLI and GUI versions
CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c

# Milestone 1: CLI Dijkstra
# Usage: ./dijkstra <input_file>
milestone1:
	$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) $(LDFLAGS) -o dijkstra $(LIBS)

# Milestone 2: Basic GUI Visualization
# Usage: ./sim <input_file>
milestone2:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# Milestone 3: Animation Simulation
# Usage: ./sim <input_file>
milestone3:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -DANIMATION -o sim $(LIBS)

# Cleanup: Removes executables and object files
clean:
	rm -f dijkstra sim *.o

.PHONY: milestone1 milestone2 milestone3 clean
