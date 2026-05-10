#include "raylib.h"
#include <math.h>
#include "graph.h"

int main() {
    InitWindow(800, 600, "Milestone 3: Animation Engine");
    SetTargetFPS(60);

    // Placeholder node positions
    Vector2 p1 = {100, 300}, p2 = {700, 300};
    Vector2 car_pos = p1;
    float progress = 0.0f;
    bool moving = false;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) moving = true;

        if (moving) {
            progress += 0.01f; // Speed
            // LERP Math: (Start + (End - Start) * progress)
            car_pos.x = p1.x + (p2.x - p1.x) * progress;
            car_pos.y = p1.y + (p2.y - p1.y) * progress;
            if (progress >= 1.0f) moving = false;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(p1, 20, RED);   // Start Node
        DrawCircleV(p2, 20, RED);   // End Node
        DrawLineV(p1, p2, GRAY);    // Edge
        
        if (moving) DrawCircleV(car_pos, 15, BLUE); // The Animated Car
        
        DrawText("Press SPACE to Animate", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
