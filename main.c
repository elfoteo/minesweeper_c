#include "grid.h"
#include "raylib.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define CELL_SIZE 40
#define CELL_PADDING 10
#define BOMBNUM 10

/* If GRID_W / GRID_H aren't in grid.h, define them here:
   #define GRID_W 10
   #define GRID_H 10
*/

Cell matrix[GRID_W][GRID_H];

/* Helper */
static inline bool is_oob(int x, int y) { return x < 0 || x >= GRID_W || y < 0 || y >= GRID_H; }

/* Flood-fill uncover for zero cells (iterative queue to avoid recursion) */
void autoexplode(int sx, int sy) {
    if (is_oob(sx, sy))
        return;
    if (matrix[sx][sy].number != 0)
        return;

    int qsize = GRID_W * GRID_H;
    int *qx = malloc(sizeof(int) * qsize);
    int *qy = malloc(sizeof(int) * qsize);
    if (!qx || !qy) {
        free(qx);
        free(qy);
        return;
    }

    int head = 0, tail = 0;
    qx[tail] = sx;
    qy[tail] = sy;
    tail++;

    while (head < tail) {
        int x = qx[head];
        int y = qy[head];
        head++;

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = x + dx;
                int ny = y + dy;
                if (is_oob(nx, ny))
                    continue;
                if (matrix[nx][ny].uncovered)
                    continue;
                if (matrix[nx][ny].flag)
                    continue; /* don't auto-uncover flagged cells */

                matrix[nx][ny].uncovered = true;

                /* If neighbor is zero, add to queue to expand */
                if (matrix[nx][ny].number == 0) {
                    /* check if already queued to avoid duplicates (simple scan) */
                    bool already = false;
                    for (int i = 0; i < tail; i++) {
                        if (qx[i] == nx && qy[i] == ny) {
                            already = true;
                            break;
                        }
                    }
                    if (!already) {
                        qx[tail] = nx;
                        qy[tail] = ny;
                        tail++;
                    }
                }
            }
        }
    }

    free(qx);
    free(qy);
}

int main(void) {
    srand((unsigned)time(NULL));

    /* Zero-init board */
    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            matrix[x][y].mine = false;
            matrix[x][y].uncovered = false;
            matrix[x][y].number = 0;
            matrix[x][y].flag = false;
        }
    }

    /* Place bombs and increment neighbour counts */
    for (int i = 0; i < BOMBNUM;) {
        int rx = rand() % GRID_W;
        int ry = rand() % GRID_H;
        if (!matrix[rx][ry].mine) {
            matrix[rx][ry].mine = true;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int ax = rx + dx;
                    int ay = ry + dy;
                    if (is_oob(ax, ay))
                        continue;
                    if (dx == 0 && dy == 0)
                        continue;
                    matrix[ax][ay].number++;
                }
            }
            i++;
        }
    }

    /* Compute window size to exactly fit the grid with padding */
    const int screenWidth = CELL_PADDING + GRID_W * (CELL_SIZE + CELL_PADDING);
    const int screenHeight = CELL_PADDING + GRID_H * (CELL_SIZE + CELL_PADDING);

    InitWindow(screenWidth, screenHeight, "Minesweeper - raylib");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse = GetMousePosition();
        bool leftClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool rightClick = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

        /* Draw grid and handle input */
        for (int gx = 0; gx < GRID_W; gx++) {
            for (int gy = 0; gy < GRID_H; gy++) {
                int px = CELL_PADDING + gx * (CELL_SIZE + CELL_PADDING);
                int py = CELL_PADDING + gy * (CELL_SIZE + CELL_PADDING);

                Color squarecolour = RED;

                Rectangle rect = {(float)px, (float)py, (float)CELL_SIZE, (float)CELL_SIZE};
                bool mouseOver = CheckCollisionPointRec(mouse, rect);

                if (matrix[gx][gy].uncovered) {
                    squarecolour = BLUE;
                    if (mouseOver)
                        squarecolour = DARKBLUE;
                } else if (matrix[gx][gy].flag) {
                    squarecolour = YELLOW;
                    if (mouseOver) {
                        squarecolour = ORANGE;
                        if (rightClick)
                            matrix[gx][gy].flag = false;
                    }
                } else if (mouseOver) {
                    squarecolour = MAROON;
                    if (leftClick) {
                        /* Uncover cell */
                        matrix[gx][gy].uncovered = true;
                        if (matrix[gx][gy].mine) {
                            /* You hit a mine — reveal all mines (simple behavior) */
                            for (int xx = 0; xx < GRID_W; xx++) {
                                for (int yy = 0; yy < GRID_H; yy++) {
                                    if (matrix[xx][yy].mine)
                                        matrix[xx][yy].uncovered = true;
                                }
                            }
                        } else if (matrix[gx][gy].number == 0) {
                            autoexplode(gx, gy);
                        }
                    } else if (rightClick) {
                        matrix[gx][gy].flag = true;
                    }
                } else {
                    squarecolour = RED;
                }

                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, squarecolour);

                if (matrix[gx][gy].uncovered) {
                    if (matrix[gx][gy].mine) {
                        DrawText("X", px + CELL_SIZE / 3, py + CELL_SIZE / 8, 30, WHITE);
                    } else {
                        const char *num = TextFormat("%i", matrix[gx][gy].number);
                        DrawText(num, px + CELL_SIZE / 3, py + CELL_SIZE / 8, 30, WHITE);
                    }
                } else if (matrix[gx][gy].flag) {
                    DrawText("F", px + CELL_SIZE / 3, py + CELL_SIZE / 8, 30, BLACK);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
