#include "grid.h"
#include "raylib.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
Cell matrix[GRID_W][GRID_H];

int inputx;
int inputy;

bool is_oob(int x, int y) { return x < 0 || x > GRID_W - 1 || y < 0 || y > GRID_H - 1; }

CellPos mouse_to_grid() {
    CellPos r;
    r.x = (int)(GetMouseX() / (CELL_SIZE + CELL_PADDING));
    r.y = (int)(GetMouseY() / (CELL_SIZE + CELL_PADDING));
    return r;
}

void grid_init() {
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
}

void grid_uncover(int x, int y) {
    if (is_oob(x, y)) {
        panic("grid_uncover was called on a cell that was out of bounds");
        return;
    }
    matrix[x][y].uncovered = true;
    if (matrix[x][y].number != 0) {
        return;
    }
    // Check 3x3 neighbours excluding the center and uncover them
    for (int x0 = -1; x0 < 2; x0++) {
        for (int y0 = -1; y0 < 2; y0++) {
            if (!is_oob(x + x0, y + y0)) { // Skip if oob
                if (!matrix[x + x0][y + y0].uncovered) {
                    grid_uncover(x + x0, y + y0);
                }
            }
        }
    }

    // if (matrix[x][y].number == 0) {
    //     matrix[x][y].fresh = 1;
    //     for (int x = 0; x < GRID_H; x++) {
    //         for (int y = 0; y < GRID_H; y++) { // For every cell in the
    //             if (matrix[x][y].fresh) {      // If it's fresh air
    //                 matrix[x][y].fresh = 0;
    //                 for (int x0 = -1; x0 < 2; x0++) {
    //                     for (int y0 = -1; y0 < 2; y0++) {              // Check in a 3x3 area its neighbours
    //                         if (!is_oob(x + x0, y + y0)) {             // Skip if oob
    //                             if (!matrix[x + x0][y + y0].uncovered) // for (int
    //                             {                                      // If its earth set it to air
    //                                 matrix[x + x0][y + y0].uncovered = 1;
    //                                 if (matrix[x + x0][y + y0].number == 0) {
    //                                     matrix[x + x0][y + y0].fresh = 1;
    //                                 }
    //                             }
    //                         }
    //                     }
    //                 }
    //                 x = 0;
    //                 y = 0;
    //             }
    //         }
    //     }
    // }
}

void grid_set_flagged(int x, int y, bool state) {
    if (is_oob(x, y)) {
        panic("grid_set_flagged was called on a cell that was out of bounds");
        return;
    }
    matrix[x][y].flag = state;
}

void grid_toggle_flag(int x, int y) { matrix[x][y].flag = !matrix[x][y].flag; }
