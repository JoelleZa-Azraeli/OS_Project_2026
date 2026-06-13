# ===============================================================
# Project: Operating Systems Simulation - Traffic on Graph
# Team: Bingo Logic
# Developers: Hiba Kljawe & Joelle Zanbil
# ===============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99   -g
CFLAGS2 = -Wall -Wextra -std=gnu99 -g

# Pointing to the Raylib folder and local src
INC     = -Isrc -I$(HOME)/raylib/include
LDFLAGS = -L$(HOME)/raylib/lib -Wl,-rpath,$(HOME)/raylib/lib
LIBS    = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Core logic compiled into every binary
CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c

# ------------------------------------------------------------------
# Milestone 1: CLI Dijkstra  ->  ./dijkstra <file>
# ------------------------------------------------------------------
milestone1:
	$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) $(LDFLAGS) -o dijkstra $(LIBS)

# ------------------------------------------------------------------
# Milestones 2 & 3: GUI + animation  ->  ./sim <file>
# ------------------------------------------------------------------
milestone2 milestone3:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ------------------------------------------------------------------
# Milestone 4: multiple processes, parent manages GUI  ->  ./sim <file>
# ------------------------------------------------------------------
milestone4:
	$(CC) $(CFLAGS2) src/main_m4.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ------------------------------------------------------------------
# Milestone 5: IPC via pipes, children autonomous  ->  ./sim <file>
# ------------------------------------------------------------------
milestone5:
	$(CC) $(CFLAGS2) src/main_m5.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ------------------------------------------------------------------
# Milestone 6: node-access synchronisation with semaphores  ->  ./sim <file>
# ------------------------------------------------------------------
milestone6:
	$(CC) $(CFLAGS2) src/main_m6.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

# ------------------------------------------------------------------
clean:
	rm -f dijkstra sim *.o

.PHONY: milestone1 milestone2 milestone3 milestone4 milestone5 milestone6 clean
