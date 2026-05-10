#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "graph_io.h"
#include "dijkstra.h"

// Timing constants for Milestone 3
#define PAUSE_TIME 1.0f    // 1 second pause at nodes
#define STEP_TIME 1.5f     // 1.5 seconds to glide between nodes (Slower)

// Function to draw directional arrows (Requirement: Milestone 2)
void DrawArrow(Vector2 start, Vector2 end, Color color) {
    DrawLineV(start, end, color);
    float angle = atan2f(end.y - start.y, end.x - start.x);
    float arrowSize = 12.0f;
    Vector2 p1 = { end.x - arrowSize * cosf(angle - PI/6), end.y - arrowSize * sinf(angle - PI/6) };
    Vector2 p2 = { end.x - arrowSize * cosf(angle + PI/6), end.y - arrowSize * sinf(angle + PI/6) };
    DrawTriangle(end, p1, p2, color);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Load graph data using teammate's logic
    Graph g;
    int startNode, endNode;
    if (!read_graph_from_file(argv[1], &g, &startNode, &endNode)) return 1;

    // Window setup
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Milestone 3: Bingo Logic Simulation");
    SetTargetFPS(60);

    // Calculate node positions in a circle
    Vector2 node_pos[MAX_NODES];
    for (int i = 0; i < g.num_nodes; i++) {
        node_pos[i] = (Vector2){
            screenWidth / 2 + 200 * cos(i * 2 * PI / g.num_nodes),
            screenHeight / 2 + 200 * sin(i * 2 * PI / g.num_nodes)
        };
    }

    // Animation state
    // Note: For now we use the path from your test case (0 -> 2 -> 5)
    int path[] = {0, 2, 5}; 
    int path_count = 3;
    Vector2 car_pos = node_pos[path[0]];
    int current_path_idx = 0;
    float timer = 0.0f;
    bool moving = false;
    bool is_pausing = true;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // UI Button logic
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){10, 10, 100, 40})) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                moving = !moving;
            }
        }

        // Milestone 3: Smooth Animation (Lerp) Logic
        if (moving && current_path_idx < path_count - 1) {
            timer += dt;

            if (is_pausing) {
                if (timer >= PAUSE_TIME) {
                    timer = 0;
                    is_pausing = false; 
                }
            } else {
                // Calculate how far we are (0.0 to 1.0)
                float fraction = timer / STEP_TIME;
                if (fraction > 1.0f) fraction = 1.0f;

                Vector2 start = node_pos[path[current_path_idx]];
                Vector2 end = node_pos[path[current_path_idx + 1]];

                // LERP: The "Gliding" math
                car_pos.x = start.x + (end.x - start.x) * fraction;
                car_pos.y = start.y + (end.y - start.y) * fraction;

                if (timer >= STEP_TIME) {
                    timer = 0;
                    current_path_idx++;
                    car_pos = node_pos[path[current_path_idx]]; // Snap to node
                    is_pausing = true; 
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw edges, arrows, and weights (Milestone 2)
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

        // Draw nodes
        for (int i = 0; i < g.num_nodes; i++) {
            DrawCircleV(node_pos[i], 22, DARKBLUE);
            DrawText(TextFormat("%d", i), node_pos[i].x - 6, node_pos[i].y - 8, 20, WHITE);
        }

        // Draw moving entity (The Red Car/Packet)
        DrawCircleV(car_pos, 12, RED);

        // Control Button
        DrawRectangleRec((Rectangle){10, 10, 100, 40}, moving ? RED : DARKGREEN);
        DrawText(moving ? "STOP" : "PLAY", 35, 20, 20, WHITE);

        if (current_path_idx >= path_count - 1) {
            DrawText("DESTINATION REACHED!", 280, 50, 25, DARKGREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
