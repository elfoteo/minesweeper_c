#include "grid.h"
#include "raylib.h"
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define XRAY false

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

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minesweeper");
    SetTargetFPS(60);

    int mode = 1;
    while (!WindowShouldClose()) {
        switch (mode) {
            case 1: {

                Color squarecolour = RED;
                BeginDrawing();
                ClearBackground(BLACK);

                CellPos mouse_pos = mouse_to_grid();
                Cell hovered_cell = matrix[mouse_pos.x][mouse_pos.y];

                for (int x = 0; x < GRID_W; x++) {
                    for (int y = 0; y < GRID_H; y++) {
                        Cell current = matrix[x][y];
                        if (current.uncovered) {
                            squarecolour = BLUE;
                            if (mouse_pos.x == x && mouse_pos.y == y)
                                squarecolour = DARKBLUE;
                            if (current.mine) {
                                squarecolour = BLACK;
                                mode = 2;
                            }
                        } else if (current.flag) {
                            squarecolour = YELLOW;
                            if (mouse_pos.x == x && mouse_pos.y == y)
                                squarecolour = ORANGE;
                        } else if (mouse_pos.x == x && mouse_pos.y == y)
                            squarecolour = MAROON;
                        else
                            squarecolour = RED;
#if XRAY
                        if (!current.uncovered) {
                            squarecolour = DARKPURPLE;
                        }
#endif
                        // Gray dropshadow
                        DrawRectangle(x * (CELL_SIZE + CELL_PADDING) + 4, y * (CELL_SIZE + CELL_PADDING) + 4, CELL_SIZE, CELL_SIZE, GRAY);
                        // Draw the tile
                        DrawRectangle(x * (CELL_SIZE + CELL_PADDING), y * (CELL_SIZE + CELL_PADDING), CELL_SIZE, CELL_SIZE, squarecolour);
                        if (current.uncovered || XRAY) {
                            const char *number = 0;
                            number = TextFormat("%i", matrix[x][y].number);
#if XRAY
                            if (current.mine) {
                                number = "*";
                            }
#endif
                            DrawText(number, x * (CELL_SIZE + CELL_PADDING) + 12, y * (CELL_PADDING + CELL_SIZE) + 7, 30, WHITE);
                        }
                    }
                }

                EndDrawing();

                if (!hovered_cell.flag && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    grid_uncover(mouse_pos.x, mouse_pos.y);
                }
                if (!hovered_cell.uncovered && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    grid_toggle_flag(mouse_pos.x, mouse_pos.y);
                }
                if (mode != 2) {
                    mode = 3;
                    for (int x = 0; x < GRID_W; x++) {
                        for (int y = 0; y < GRID_H; y++) {
                            if (!matrix[x][y].uncovered && !matrix[x][y].mine)
                                mode = 1;
                        }
                    }
                }
                break;
            }
            case 2: {
                int fontsize = SCREEN_WIDTH / 21;

                BeginDrawing();
                ClearBackground(BLACK); // TODO change this for something better

                DrawText("YOU LOST", 0, 0, 4 * fontsize, GRAY);
                DrawText("Nothing left to do but to close the window.", 0, 4 * fontsize, fontsize, GRAY);

                EndDrawing();
                break;
            }

            case 3: {
                int fontsizewin = SCREEN_WIDTH / 20;

                BeginDrawing();
                ClearBackground(BLACK); // TODO change this for something better

                DrawText("YOU WON", 0, 0, 4 * fontsizewin, YELLOW);
                DrawText("As a reward, you can close this window.", 0, 4 * fontsizewin, fontsizewin, YELLOW);

                EndDrawing();
            }
        }
    }

    CloseWindow();
    return 0;
}
