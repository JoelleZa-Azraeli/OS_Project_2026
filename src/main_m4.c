#define _POSIX_C_SOURCE 200809L
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "graph.h"
#include "graph_io.h"
#include "dijkstra.h"
#include "travelers.h"

#define PAUSE_TIME 1.0f
#define STEP_TIME  0.3f

static Color TRAVELER_COLORS[] = {
    {220,  80,  80, 255},   // soft red
    { 70, 130, 220, 255},   // soft blue
    { 60, 180,  90, 255},   // soft green
    {170,  80, 200, 255},   // soft purple
    {220, 160,  50, 255},   // gold
    {200,  90, 150, 255},   // soft pink
    { 80, 190, 190, 255},   // teal
    {210, 130,  60, 255},   // soft orange
    {100, 160, 220, 255},   // sky blue
    {140, 100, 200, 255},   // violet
};

typedef struct {
    int    path[MAX_NODES];
    int    path_count;
    int    current_step;
    int    current_jump;
    float  timer;
    bool   is_pausing;
    bool   reached;
    Vector2 pos;
    pid_t  pid;
} TravelerAnim;

static void DrawArrow(Vector2 start, Vector2 end, Color color, float thick) {
    DrawLineEx(start, end, thick, color);
    float angle = atan2f(end.y - start.y, end.x - start.x);
    float sz = 14.0f;
    Vector2 p1 = { end.x - sz * cosf(angle - PI/6), end.y - sz * sinf(angle - PI/6) };
    Vector2 p2 = { end.x - sz * cosf(angle + PI/6), end.y - sz * sinf(angle + PI/6) };
    DrawTriangle(end, p1, p2, color);
}

int main(int argc, char* argv[]) {
    if (argc < 2) { printf("Usage: %s <input_file>\n", argv[0]); return 1; }

    Graph g;
    Traveler travelers[MAX_TRAVELERS];
    int num_travelers = 0;
    if (!read_graph_with_travelers(argv[1], &g, travelers, &num_travelers)) {
        printf("Error: Could not read file.\n"); return 1;
    }

    // Parent computes all paths before forking
    TravelerAnim anims[MAX_TRAVELERS];
    for (int i = 0; i < num_travelers; i++) {
        anims[i].path_count   = compute_path(&g, travelers[i].src, travelers[i].dst, anims[i].path);
        anims[i].current_step = 0;
        anims[i].current_jump = 0;
        anims[i].timer        = 0.0f;
        anims[i].is_pausing   = false;
        anims[i].reached      = (anims[i].path_count <= 1);
        anims[i].pid          = -1;
    }

    // Fork children — they just print [PID] started and sleep until signalled
    for (int i = 0; i < num_travelers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("[%d] started\n", getpid());
            fflush(stdout);
            while (1) sleep(1);
            exit(0);
        }
        anims[i].pid = pid;
    }

    // GUI setup (after fork so children don't inherit the window)
    const int screenWidth  = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Bingo Logic - Graph Simulation (M4)");
    SetTargetFPS(60);

    Vector2 node_pos[MAX_NODES];
    for (int i = 0; i < g.num_nodes; i++) {
        node_pos[i] = (Vector2){
            screenWidth  / 2 + 200 * cosf(i * 2 * PI / g.num_nodes),
            screenHeight / 2 + 200 * sinf(i * 2 * PI / g.num_nodes)
        };
    }

    for (int i = 0; i < num_travelers; i++)
        anims[i].pos = (anims[i].path_count > 0) ? node_pos[anims[i].path[0]] : (Vector2){0,0};

    bool moving = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, (Rectangle){10, 10, 100, 40}))
                moving = !moving;
        }

        if (moving) {
            for (int i = 0; i < num_travelers; i++) {
                TravelerAnim* a = &anims[i];
                if (a->reached || a->path_count <= 1) continue;

                a->timer += dt;

                if (a->is_pausing) {
                    if (a->timer >= PAUSE_TIME) { a->timer = 0; a->is_pausing = false; }
                } else {
                    int u = a->path[a->current_step];
                    int v = a->path[a->current_step + 1];
                    int w = g.weights[u][v];

                    if (a->timer >= STEP_TIME) {
                        a->timer = 0;
                        a->current_jump++;

                        Vector2 sp = node_pos[u], ep = node_pos[v];
                        a->pos.x = sp.x + (ep.x - sp.x) * ((float)a->current_jump / w);
                        a->pos.y = sp.y + (ep.y - sp.y) * ((float)a->current_jump / w);

                        if (a->current_jump >= w) {
                            a->current_step++;
                            a->current_jump = 0;
                            if (a->current_step >= a->path_count - 1) {
                                a->reached = true;
                                kill(a->pid, SIGTERM);
                            } else {
                                a->is_pausing = true;
                                a->pos = node_pos[a->path[a->current_step]];
                            }
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground((Color){245, 245, 250, 255});

        // Base edges (gray, thick)
        for (int i = 0; i < g.num_nodes; i++)
            for (int j = 0; j < g.num_nodes; j++)
                if (g.weights[i][j] != -1)
                    DrawArrow(node_pos[i], node_pos[j], (Color){160, 160, 175, 255}, 2.5f);

        // Edge weight labels
        for (int i = 0; i < g.num_nodes; i++)
            for (int j = 0; j < g.num_nodes; j++)
                if (g.weights[i][j] != -1)
                    DrawText(TextFormat("%d", g.weights[i][j]),
                        (int)((node_pos[i].x + node_pos[j].x) / 2) + 6,
                        (int)((node_pos[i].y + node_pos[j].y) / 2) + 6, 15, (Color){80, 80, 100, 255});

        // Nodes
        for (int i = 0; i < g.num_nodes; i++) {
            DrawCircleV(node_pos[i], 22, (Color){50, 70, 120, 255});
            DrawText(TextFormat("%d", i), (int)node_pos[i].x - 6, (int)node_pos[i].y - 8, 20, WHITE);
        }

        // Travelers — colored dots with white outline
        // When pausing at a node, apply a small per-traveler offset so they don't fully overlap
        for (int i = 0; i < num_travelers; i++) {
            if (anims[i].path_count > 0) {
                Vector2 pos = anims[i].pos;
                if (anims[i].is_pausing || anims[i].reached) {
                    pos.x += (float)((i % 3) - 1) * 10;
                    pos.y += (float)(i / 3) * 10;
                }
                DrawCircleV(pos, 13, TRAVELER_COLORS[i % 10]);
                DrawCircleLines((int)pos.x, (int)pos.y, 13, WHITE);
            }
        }

        // Play/Stop button
        DrawRectangleRec((Rectangle){10, 10, 100, 40}, moving ? (Color){180,50,50,255} : (Color){50,140,80,255});
        DrawText(moving ? "STOP" : "PLAY", 35, 20, 20, WHITE);

        // Legend — shows which color goes where
        int lx = screenWidth - 190, ly = 10;
        DrawRectangle(lx - 8, ly - 6, 188, num_travelers * 28 + 12, (Color){230,230,240,220});
        for (int i = 0; i < num_travelers; i++) {
            Color c = TRAVELER_COLORS[i % 10];
            DrawCircle(lx + 8, ly + 10 + i * 28, 9, c);
            DrawCircleLines(lx + 8, ly + 10 + i * 28, 9, WHITE);
            DrawText(TextFormat("%d -> %d", travelers[i].src, travelers[i].dst),
                lx + 22, ly + 3 + i * 28, 18, (Color){30,30,30,255});
        }

        bool all_done = true;
        for (int i = 0; i < num_travelers; i++)
            if (!anims[i].reached && anims[i].path_count > 1) { all_done = false; break; }
        if (all_done && moving) {
            DrawText("ALL TRAVELERS DONE!", 255, 45, 24, DARKGREEN);
            DrawText("Press ESC to close", 285, 75, 18, (Color){100, 100, 100, 255});
        }

        EndDrawing();
    }

    CloseWindow();

    // Wait for all children (they may already be dead from SIGTERM)
    for (int i = 0; i < num_travelers; i++) {
        if (anims[i].pid > 0) {
            kill(anims[i].pid, SIGTERM);
            waitpid(anims[i].pid, NULL, 0);
        }
    }

    return 0;
}
