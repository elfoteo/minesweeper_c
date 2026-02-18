#include "grid.h"
#include "raylib.h"
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

#define CELL_SIZE 40
#define CELL_PADDING 10
#define BOMBNUM 10

Cell matrix[GRID_W][GRID_H];
bool running = true;

bool is_oob(int x, int y) { return x < 0 || x > GRID_W - 1 || y < 0 || y > GRID_H - 1; }
int inputx;
int inputy;

void autoexplode() {
    if (matrix[inputx][inputy].number == 0) {
        matrix[inputx][inputy].fresh = 1;
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

    while (!WindowShouldClose()) {
        Color squarecolour = RED;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Vector2 mouse = GetMousePosition();

        // TODO: x and y here refer to the screen coords, not to the grid
        for (int x = CELL_PADDING; x < SCREEN_WIDTH; x += CELL_SIZE + CELL_PADDING) {
            for (int y = CELL_PADDING; y < SCREEN_HEIGHT; y += CELL_SIZE + CELL_PADDING) {
                Vector2 squarelocation = {(float)x, (float)y};
                if (matrix[x][y].uncovered) {
                    squarecolour = BLUE;
                    if (mouse.x > squarelocation.x && mouse.y > squarelocation.y && mouse.x < squarelocation.x + CELL_SIZE &&
                        mouse.y < squarelocation.y + CELL_SIZE) {
                        squarecolour = DARKBLUE;
                    }
                } else if (matrix[x][y].flag) {
                    squarecolour = YELLOW;
                    if (mouse.x > squarelocation.x && mouse.y > squarelocation.y && mouse.x < squarelocation.x + CELL_SIZE &&
                        mouse.y < squarelocation.y + CELL_SIZE) {
                        squarecolour = ORANGE;
                        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
                            matrix[x][y].flag = false;
                    }
                } else if (mouse.x > squarelocation.x && mouse.y > squarelocation.y && mouse.x < squarelocation.x + CELL_SIZE &&
                           mouse.y < squarelocation.y + CELL_SIZE) {
                    squarecolour = MAROON;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        matrix[x][y].uncovered = true;
                        inputx = x;
                        inputy = y;
                        // autoexplode(); TODO fix this bs
                    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                        matrix[x][y].flag = true;
                    }
                } else
                    squarecolour = RED;
                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, squarecolour);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
