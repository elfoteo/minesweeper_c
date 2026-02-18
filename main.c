#include "grid.h"
#include "raylib.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

Cell matrix[GRID_W][GRID_H];
bool running = true;

int inputx;
int inputy;

void autoexplode() {
    if (matrix[GetMouseGridValues().x][GetMouseGridValues().y].number == 0) {
        matrix[GetMouseGridValues().x][GetMouseGridValues().y].fresh = 1;
        for (int x = 0; x < GRID_H; x++) {
            for (int y = 0; y < GRID_H; y++) { // For every cell in the
                if (matrix[x][y].fresh) {      // If it's fresh air
                    matrix[x][y].fresh = 0;
                    for (int x0 = -1; x0 < 2; x0++) {
                        for (int y0 = -1; y0 < 2; y0++) {              // Check in a 3x3 area its neighbours
                            if (!is_oob(x + x0, y + y0)) {             // Skip if oob
                                if (!matrix[x + x0][y + y0].uncovered) // for (int
                                {                                      // If its earth set it to air
                                    matrix[x + x0][y + y0].uncovered = 1;
                                    if (matrix[x + x0][y + y0].number == 0) {
                                        matrix[x + x0][y + y0].fresh = 1;
                                    }
                                }
                            }
                        }
                    }
                    x = 0;
                    y = 0;
                }
            }
        }
    }
}

int main() {
    srand(time(NULL));

    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            matrix[x][y].mine = 0;
            matrix[x][y].uncovered = 0;
            matrix[x][y].number = 0;
            matrix[x][y].fresh = 0;
        }
    }

    for (int i = 0; i < BOMBNUM;) {
        int rand_x = rand() % GRID_W;
        int rand_y = rand() % GRID_H;
        if (!matrix[rand_x][rand_y].mine) {
            matrix[rand_x][rand_y].mine = true;
            // Loop all surrounding tiles and tile.number++;
            for (int x = -1; x < 2; x++) {
                for (int y = -1; y < 2; y++) {
                    int ax = rand_x + x;
                    int ay = rand_y + y;
                    if (is_oob(ax, ay) || (x == 0 && y == 0)) {
                        continue; // Skip this iteration of the loop
                    }
                    matrix[ax][ay].number++;
                }
            }
            i++;
        }
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);

    bool running = true;
    while (!WindowShouldClose()) {
        while (running && !WindowShouldClose()) {
            Color squarecolour = RED;
            BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int x = 0; x < GRID_W; x++) {
                for (int y = 0; y < GRID_H; y++) {
                    if (matrix[x][y].uncovered) {
                        squarecolour = BLUE;
                        if (GetMouseGridValues().x == x && GetMouseGridValues().y == y)
                            squarecolour = DARKBLUE;
                        if (matrix[x][y].mine) {
                            squarecolour = BLACK;
                            running = false;
                        }
                    } else if (matrix[x][y].flag) {
                        squarecolour = YELLOW;
                        if (GetMouseGridValues().x == x && GetMouseGridValues().y == y)
                            squarecolour = ORANGE;
                    } else if (GetMouseGridValues().x == x && GetMouseGridValues().y == y)
                        squarecolour = MAROON;
                    else
                        squarecolour = RED;
                    DrawRectangle(x * (CELL_SIZE + CELL_PADDING) + 4, y * (CELL_SIZE + CELL_PADDING) + 4, CELL_SIZE, CELL_SIZE, GRAY);
                    DrawRectangle(x * (CELL_SIZE + CELL_PADDING), y * (CELL_SIZE + CELL_PADDING), CELL_SIZE, CELL_SIZE, squarecolour);
                    if (matrix[x][y].uncovered) {
                        const char *number = 0;
                        number = TextFormat("%i", matrix[x][y].number);
                        DrawText(number, x * (CELL_SIZE + CELL_PADDING) + 12, y * (CELL_PADDING + CELL_SIZE) + 7, 30, WHITE);
                    }
                }
            }

            EndDrawing();

            if (!matrix[GetMouseGridValues().x][GetMouseGridValues().y].flag && GetMouseGridValues().left) {
                matrix[GetMouseGridValues().x][GetMouseGridValues().y].uncovered = true;
                autoexplode();
            }
            if (!matrix[GetMouseGridValues().x][GetMouseGridValues().y].uncovered && GetMouseGridValues().right)
                matrix[GetMouseGridValues().x][GetMouseGridValues().y].flag = true;

            if (WindowShouldClose())
                CloseWindow();
        }

        int fontsize = SCREEN_WIDTH / 21;

        BeginDrawing();
        ClearBackground(RAYWHITE); // TODO change this for something better

        DrawText("YOU LOST", 0, 0, 4 * fontsize, BLACK);
        DrawText("Nothing left to do but to close the window.", 0, 4 * fontsize, fontsize, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
