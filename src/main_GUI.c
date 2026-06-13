#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "graph_io.h"
#include "dijkstra.h"

// Milestone 3: Timing constants
#define PAUSE_TIME 1.0f    // 1 second pause at each node
#define STEP_TIME 0.3f     // 300ms per movement jump

// External variables from dijkstra.c to handle dynamic pathing
extern int final_path[MAX_NODES];
extern int final_path_count;

// Milestone 2: Function to draw directional arrows for the graph edges
void DrawArrow(Vector2 start, Vector2 end, Color color) {
    DrawLineV(start, end, color);
    float angle = atan2f(end.y - start.y, end.x - start.x);
    float arrowSize = 12.0f;
    Vector2 p1 = { end.x - arrowSize * cosf(angle - PI/6), end.y - arrowSize * sinf(angle - PI/6) };
    Vector2 p2 = { end.x - arrowSize * cosf(angle + PI/6), end.y - arrowSize * sinf(angle + PI/6) };
    DrawTriangle(end, p1, p2, color);
}

int main(int argc, char* argv[]) {
    // Check for required input file argument
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Load graph and query data from the text file
    Graph g;
    int startNode, endNode;
    if (!read_graph_from_file(argv[1], &g, &startNode, &endNode)) {
        printf("Error: Could not read graph file.\n");
        return 1;
    }

    // Dynamic Logic: Run Dijkstra to populate final_path array
    find_shortest_path(&g, startNode, endNode);

    // GUI Window Setup
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Bingo Logic - Graph Simulation");
    SetTargetFPS(60);

    // Calculate circular positions for graph nodes
    Vector2 node_pos[MAX_NODES];
    for (int i = 0; i < g.num_nodes; i++) {
        node_pos[i] = (Vector2){
            screenWidth / 2 + 200 * cos(i * 2 * PI / g.num_nodes),
            screenHeight / 2 + 200 * sin(i * 2 * PI / g.num_nodes)
        };
    }

    // Animation state variables
    int current_step = 0;
    int current_jump = 0; // Tracks the W jumps between nodes
    Vector2 entity_pos = (final_path_count > 0) ? node_pos[final_path[0]] : (Vector2){0, 0};
    float timer = 0.0f;
    bool moving = false;
    bool is_pausing = false;
    bool reached = false;

    // Main GUI Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Handle Play/Stop button clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            if (CheckCollisionPointRec(mouse, (Rectangle){10, 10, 100, 40})) {
                if (final_path_count > 0) moving = !moving;
            }
        }

        // Milestone 3: Animation Engine (Dynamic Path with W Jumps)
        if (moving && !reached && current_step < final_path_count - 1) {
            timer += dt;

            if (is_pausing) {
                // Mandatory 1-second pause at nodes
                if (timer >= PAUSE_TIME) {
                    timer = 0;
                    is_pausing = false;
                }
            } else {
                // Move in W jumps based on edge weight
                int u = final_path[current_step];
                int v = final_path[current_step + 1];
                int w = g.weights[u][v];

                if (timer >= STEP_TIME) {
                    timer = 0;
                    current_jump++;

                    // Interpolate position based on current jump out of W
                    Vector2 start_pos = node_pos[u];
                    Vector2 end_pos = node_pos[v];
                    entity_pos.x = start_pos.x + (end_pos.x - start_pos.x) * ((float)current_jump / w);
                    entity_pos.y = start_pos.y + (end_pos.y - start_pos.y) * ((float)current_jump / w);

                    // If we completed all W jumps for this edge
                    if (current_jump >= w) {
                        current_step++;
                        current_jump = 0; // Reset jumps for the next edge

                        if (current_step >= final_path_count - 1) {
                            reached = true;
                        } else {
                            is_pausing = true; // Pause at the new node
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw Graph Edges and Weights
        for (int i = 0; i < g.num_nodes; i++) {
            for (int j = 0; j < g.num_nodes; j++) {
                if (g.weights[i][j] != -1) {
                    DrawArrow(node_pos[i], node_pos[j], DARKGRAY);
                    DrawText(TextFormat("%d", g.weights[i][j]),
                             (node_pos[i].x + node_pos[j].x)/2 + 5,
                             (node_pos[i].y + node_pos[j].y)/2 + 5, 15, MAROON);
                }
            }
        }

        // Draw Graph Nodes
        for (int i = 0; i < g.num_nodes; i++) {
            DrawCircleV(node_pos[i], 22, DARKBLUE);
            DrawText(TextFormat("%d", i), node_pos[i].x - 6, node_pos[i].y - 8, 20, WHITE);
        }

        // Draw the moving entity (red circle)
        if (final_path_count > 0) {
            DrawCircleV(entity_pos, 12, RED);
        } else {
            DrawText("No Path to Animate", 300, 20, 20, RED);
        }

        // UI: Play/Stop Button
        DrawRectangleRec((Rectangle){10, 10, 100, 40}, moving ? RED : DARKGREEN);
        DrawText(moving ? "STOP" : "PLAY", 35, 20, 20, WHITE);

        // UI: Success Message
        if (reached) {
            DrawText("DESTINATION REACHED!", 280, 50, 25, DARKGREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
