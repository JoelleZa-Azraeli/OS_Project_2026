#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include "graph.h"

int main() {
    // 1. Setup Window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Milestone 3: Bingo Logic Traffic Sim");
    SetTargetFPS(60);

    // 2. Placeholder Data (Partner will provide real path later)
    int path[] = {0, 1, 2, 3}; // Example path
    int path_count = 4;
    Vector2 node_pos[] = {{100, 300}, {300, 100}, {500, 500}, {700, 300}};

    // 3. Animation State
    Vector2 car_pos = node_pos[path[0]];
    int current_idx = 0;
    float progress = 0.0f;
    float speed = 0.01f;
    bool moving = false;

    while (!WindowShouldClose()) {
        // --- UPDATE LOGIC ---
        if (IsKeyPressed(KEY_SPACE)) moving = true;
        if (IsKeyPressed(KEY_R)) { // Reset logic
            moving = false;
            progress = 0.0f;
            current_idx = 0;
            car_pos = node_pos[path[0]];
        }

        if (moving && current_idx < path_count - 1) {
            progress += speed;
            Vector2 start = node_pos[path[current_idx]];
            Vector2 end = node_pos[path[current_idx + 1]];

            // LERP Math for smooth movement
            car_pos.x = start.x + (end.x - start.x) * progress;
            car_pos.y = start.y + (end.y - start.y) * progress;

            if (progress >= 1.0f) {
                progress = 0.0f;
                current_idx++;
                if (current_idx >= path_count - 1) moving = false;
            }
        }

        // --- DRAW LOGIC ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw All Edges
        for (int i = 0; i < path_count - 1; i++) {
            DrawLineEx(node_pos[path[i]], node_pos[path[i+1]], 2, LIGHTGRAY);
        }

        // Draw Shortest Path Highlighting (Milestone 3 feature)
        for (int i = 0; i < current_idx; i++) {
            DrawLineEx(node_pos[path[i]], node_pos[path[i+1]], 4, GREEN);
        }

        // Draw Nodes
        for (int i = 0; i < 4; i++) {
            DrawCircleV(node_pos[i], 20, RED);
            DrawText(TextFormat("%d", i), node_pos[i].x - 5, node_pos[i].y - 5, 20, WHITE);
        }

        // Draw the Animated "Car"
        DrawCircleV(car_pos, 12, BLUE);

        DrawText("SPACE: Start | R: Reset | Speed: 0.01", 10, 10, 20, DARKGRAY);
        if (!moving && current_idx >= path_count - 1) DrawText("DESTINATION REACHED!", 300, 550, 20, GREEN);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
