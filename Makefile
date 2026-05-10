# ===============================================================
# Project: Operating Systems Simulation - Traffic on Graph
# Team: Bingo Logic
# ===============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
# Pointing to the Raylib you just downloaded
INC = -Isrc -I$(HOME)/raylib/include
LDFLAGS = -L$(HOME)/raylib/lib
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CORE_SRC = src/graph.c src/graph_io.c src/dijkstra.c

milestone1:
	$(CC) $(CFLAGS) src/main_dijkstra.c $(CORE_SRC) $(INC) -o dijkstra

milestone2:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -o sim $(LIBS)

milestone3:
	$(CC) $(CFLAGS) src/main_GUI.c $(CORE_SRC) $(INC) $(LDFLAGS) -DANIMATION -o sim $(LIBS)

clean:
	rm -f dijkstra sim *.o
