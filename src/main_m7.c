#define _POSIX_C_SOURCE 200809L
#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "graph.h"
#include "graph_io.h"
#include "dijkstra.h"
#include "travelers.h"

/* ---------- M7 IPC message types ---------- */
#define M7_WAITING  0   /* child: "I want to enter this node" */
#define M7_ARRIVED  1   /* child: "parent let me in, I'm inside" */
#define M7_LEAVING  2   /* child: "I'm done with this node, schedule next waiter" */
#define M7_FINISHED 3   /* child: "I reached my destination" */

typedef struct {
    int msg_type;
    int traveler_idx;
    int node;
    int next_node;
    int remaining_hops;  /* hops left in path — used by SJF priority */
} M7Msg;

/* Shared memory: one personal semaphore per traveler.
   Starts at 0 so child immediately blocks after sending M7_WAITING.
   Parent posts it when the child wins the scheduling decision. */
typedef struct {
    sem_t personal[MAX_TRAVELERS];
} M7Shared;

/* ---------- Scheduling ---------- */
typedef enum { FCFS, SJF } SchedAlgo;

typedef struct {
    int idx[MAX_TRAVELERS];   /* traveler indices queued for this node */
    int hops[MAX_TRAVELERS];  /* their remaining_hops (for SJF) */
    int count;
    int busy;
} NodeQueue;

/* Pick winner from queue, post their semaphore so they can enter. */
static void grant_node(int node, NodeQueue *queues, M7Shared *shared, SchedAlgo algo) {
    NodeQueue *q = &queues[node];
    if (q->count == 0) { q->busy = 0; return; }

    int pick = 0;  /* FCFS: oldest arrival = index 0 */
    if (algo == SJF) {
        for (int i = 1; i < q->count; i++)
            if (q->hops[i] < q->hops[pick]) pick = i;
    }

    int winner = q->idx[pick];
    for (int i = pick; i < q->count - 1; i++) {
        q->idx[i]  = q->idx[i + 1];
        q->hops[i] = q->hops[i + 1];
    }
    q->count--;
    q->busy = 1;
    sem_post(&shared->personal[winner]);
}

/* ---------- Visuals ---------- */
static Color TRAVELER_COLORS[] = {
    {220,  80,  80, 255}, { 70, 130, 220, 255}, { 60, 180,  90, 255},
    {170,  80, 200, 255}, {220, 160,  50, 255}, {200,  90, 150, 255},
    { 80, 190, 190, 255}, {210, 130,  60, 255}, {100, 160, 220, 255},
    {140, 100, 200, 255},
};

static void DrawArrow(Vector2 start, Vector2 end, Color color, float thick) {
    DrawLineEx(start, end, thick, color);
    float angle = atan2f(end.y - start.y, end.x - start.x);
    float sz = 14.0f;
    Vector2 p1 = {end.x - sz * cosf(angle - PI/6), end.y - sz * sinf(angle - PI/6)};
    Vector2 p2 = {end.x - sz * cosf(angle + PI/6), end.y - sz * sinf(angle + PI/6)};
    DrawTriangle(end, p1, p2, color);
}

typedef struct {
    int read_fd;
    int current_node;
    int waiting_for;
    int finished;
} TState;

int main(int argc, char *argv[]) {
    if (argc < 4 || strcmp(argv[1], "-schd") != 0) {
        printf("Usage: %s -schd <fcfs|sjf> <input_file>\n", argv[0]);
        return 1;
    }

    SchedAlgo algo;
    if      (strcmp(argv[2], "fcfs") == 0) algo = FCFS;
    else if (strcmp(argv[2], "sjf")  == 0) algo = SJF;
    else {
        printf("Unknown scheduler '%s'. Use fcfs or sjf.\n", argv[2]);
        return 1;
    }
    const char *algo_name = (algo == FCFS) ? "FCFS" : "SJF";

    Graph g;
    Traveler travelers[MAX_TRAVELERS];
    int num_travelers = 0;
    if (!read_graph_with_travelers(argv[3], &g, travelers, &num_travelers)) {
        printf("Error: Could not read file.\n");
        return 1;
    }

    /* Personal semaphores in shared memory — parent posts, child waits */
    M7Shared *shared = mmap(NULL, sizeof(M7Shared),
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) { perror("mmap"); return 1; }
    for (int i = 0; i < num_travelers; i++)
        sem_init(&shared->personal[i], 1, 0);

    int pipes[MAX_TRAVELERS][2];
    for (int i = 0; i < num_travelers; i++) pipe(pipes[i]);

    pid_t pids[MAX_TRAVELERS];
    for (int i = 0; i < num_travelers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            for (int j = 0; j < num_travelers; j++) {
                close(pipes[j][0]);
                if (j != i) close(pipes[j][1]);
            }
            int wfd = pipes[i][1];

            int path[MAX_NODES];
            int path_count = compute_path(&g, travelers[i].src, travelers[i].dst, path);

            M7Msg msg;
            msg.traveler_idx = i;

            for (int step = 0; step < path_count; step++) {
                /* Ask parent for permission to enter this node */
                msg.msg_type       = M7_WAITING;
                msg.node           = path[step];
                msg.next_node      = (step < path_count - 1) ? path[step + 1] : -1;
                msg.remaining_hops = path_count - step;
                write(wfd, &msg, sizeof(msg));

                /* Block until parent grants access */
                sem_wait(&shared->personal[i]);

                /* Now inside the node */
                msg.msg_type = M7_ARRIVED;
                write(wfd, &msg, sizeof(msg));

                sleep(1);

                /* Notify parent we're leaving so it can let the next waiter in */
                msg.msg_type = M7_LEAVING;
                write(wfd, &msg, sizeof(msg));

                if (step < path_count - 1)
                    usleep(g.weights[path[step]][path[step + 1]] * 300000);
            }

            msg.msg_type = M7_FINISHED;
            write(wfd, &msg, sizeof(msg));
            close(wfd);
            exit(0);
        }
        pids[i] = pid;
        close(pipes[i][1]);
    }

    TState states[MAX_TRAVELERS];
    for (int i = 0; i < num_travelers; i++) {
        states[i].read_fd      = pipes[i][0];
        states[i].current_node = travelers[i].src;
        states[i].waiting_for  = -1;
        states[i].finished     = 0;
        fcntl(pipes[i][0], F_SETFL, O_NONBLOCK);
    }

    NodeQueue queues[MAX_NODES];
    for (int i = 0; i < g.num_nodes; i++) { queues[i].count = 0; queues[i].busy = 0; }

    const int W = 800, H = 600;
    char title[64];
    snprintf(title, sizeof(title), "Bingo Logic - M7 Scheduling (%s)", algo_name);
    InitWindow(W, H, title);
    SetTargetFPS(60);

    Vector2 node_pos[MAX_NODES];
    for (int i = 0; i < g.num_nodes; i++) {
        node_pos[i] = (Vector2){
            W / 2 + 200 * cosf(i * 2 * PI / g.num_nodes),
            H / 2 + 200 * sinf(i * 2 * PI / g.num_nodes)
        };
    }

    float wx[] = {-35,  35, -35,  35,   0};
    float wy[] = {-35, -35,  35, -35,  40};

    while (!WindowShouldClose()) {
        /* Drain all child pipes */
        for (int i = 0; i < num_travelers; i++) {
            if (states[i].finished) continue;
            M7Msg msg;
            while (read(states[i].read_fd, &msg, sizeof(msg)) == (ssize_t)sizeof(msg)) {
                int nd = msg.node;

                if (msg.msg_type == M7_WAITING) {
                    states[i].waiting_for = nd;
                    NodeQueue *q = &queues[nd];
                    q->idx[q->count]  = i;
                    q->hops[q->count] = msg.remaining_hops;
                    q->count++;
                    /* If no one is inside this node yet, grant immediately */
                    if (!q->busy) grant_node(nd, queues, shared, algo);

                } else if (msg.msg_type == M7_ARRIVED) {
                    states[i].current_node = nd;
                    states[i].waiting_for  = -1;
                    if (msg.next_node == -1)
                        printf("[%s][PID=%d] arrived at node %d | DESTINATION\n",
                               algo_name, pids[i], nd);
                    else
                        printf("[%s][PID=%d] arrived at node %d | next: %d\n",
                               algo_name, pids[i], nd, msg.next_node);
                    fflush(stdout);

                } else if (msg.msg_type == M7_LEAVING) {
                    /* Node is now free — pick next from queue */
                    grant_node(nd, queues, shared, algo);

                } else if (msg.msg_type == M7_FINISHED) {
                    printf("[%s][PID=%d] finished\n", algo_name, pids[i]);
                    fflush(stdout);
                    states[i].finished    = 1;
                    states[i].waiting_for = -1;
                    close(states[i].read_fd);
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground((Color){245, 245, 250, 255});

        /* Edges */
        for (int i = 0; i < g.num_nodes; i++)
            for (int j = 0; j < g.num_nodes; j++)
                if (g.weights[i][j] != -1)
                    DrawArrow(node_pos[i], node_pos[j], (Color){160, 160, 175, 255}, 2.5f);

        /* Edge weights */
        for (int i = 0; i < g.num_nodes; i++)
            for (int j = 0; j < g.num_nodes; j++)
                if (g.weights[i][j] != -1)
                    DrawText(TextFormat("%d", g.weights[i][j]),
                        (int)((node_pos[i].x + node_pos[j].x) / 2) + 6,
                        (int)((node_pos[i].y + node_pos[j].y) / 2) + 6,
                        15, (Color){80, 80, 100, 255});

        /* Nodes */
        for (int i = 0; i < g.num_nodes; i++) {
            DrawCircleV(node_pos[i], 22, (Color){50, 70, 120, 255});
            DrawText(TextFormat("%d", i),
                (int)node_pos[i].x - 6, (int)node_pos[i].y - 8, 20, WHITE);
        }

        /* Travelers */
        for (int i = 0; i < num_travelers; i++) {
            Color c;
            Vector2 pos;
            if (states[i].waiting_for != -1) {
                c   = (Color){255, 165, 0, 255};
                int wn = states[i].waiting_for;
                pos = (Vector2){node_pos[wn].x + wx[i%5], node_pos[wn].y + wy[i%5]};
            } else {
                c   = TRAVELER_COLORS[i % 10];
                pos = node_pos[states[i].current_node];
                pos.x += (float)((i % 3) - 1) * 10;
                pos.y += (float)(i / 3) * 10;
            }
            DrawCircleV(pos, 13, c);
            DrawCircleLines((int)pos.x, (int)pos.y, 13, WHITE);
        }

        /* Scheduler label (top-left) */
        DrawRectangle(10, 10, 210, 36, (Color){50, 70, 120, 220});
        DrawText(TextFormat("Scheduler: %s", algo_name), 18, 20, 18, WHITE);

        /* Legend (top-right) */
        int lx = W - 190, ly = 10;
        DrawRectangle(lx - 8, ly - 6, 188, num_travelers * 28 + 40, (Color){230, 230, 240, 220});
        for (int i = 0; i < num_travelers; i++) {
            DrawCircle(lx + 8, ly + 10 + i * 28, 9, TRAVELER_COLORS[i % 10]);
            DrawCircleLines(lx + 8, ly + 10 + i * 28, 9, WHITE);
            DrawText(TextFormat("%d -> %d", travelers[i].src, travelers[i].dst),
                lx + 22, ly + 3 + i * 28, 18, (Color){30, 30, 30, 255});
        }
        int oy = ly + num_travelers * 28 + 8;
        DrawCircle(lx + 8, oy + 6, 9, (Color){255, 165, 0, 255});
        DrawCircleLines(lx + 8, oy + 6, 9, WHITE);
        DrawText("= waiting", lx + 22, oy, 15, (Color){80, 80, 80, 255});

        bool all_done = true;
        for (int i = 0; i < num_travelers; i++)
            if (!states[i].finished) { all_done = false; break; }
        if (all_done) {
            DrawText("ALL TRAVELERS DONE!", 255, 45, 24, DARKGREEN);
            DrawText("Press ESC to close",  285, 75, 18, (Color){100, 100, 100, 255});
        }

        EndDrawing();
    }

    CloseWindow();
    for (int i = 0; i < num_travelers; i++) waitpid(pids[i], NULL, 0);
    for (int i = 0; i < num_travelers; i++) sem_destroy(&shared->personal[i]);
    munmap(shared, sizeof(M7Shared));
    return 0;
}
