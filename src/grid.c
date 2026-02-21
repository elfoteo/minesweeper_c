#include "grid.h"
#include "raylib.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
Cell matrix[GRID_W][GRID_H];
bool initialized = false;

bool is_oob(int x, int y) { return x < 0 || x > GRID_W - 1 || y < 0 || y > GRID_H - 1; }

static void clean_grid() {
    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            matrix[x][y].mine = 0;
            matrix[x][y].uncovered = 0;
            matrix[x][y].number = 0;
            matrix[x][y].flag = 0;
        }
    }
}

void grid_init(int safe_x, int safe_y) {
    srand(time(NULL));
    if (initialized) {
        panic("Grid already initialized\n");
    }

    if (BOMBNUM > GRID_W * GRID_H - 1) {
        panic("Too many bombs for the grid size!\n");
    }
    initialized = true;
    clean_grid();

    int bombs = 0;
    do {
        int rand_x = rand() % GRID_W;
        int rand_y = rand() % GRID_H;
        // Skip placing the bomb and try to find a new location if
        // - A mine is already there
        // - It's the safe spot
        if (!matrix[rand_x][rand_y].mine && rand_x != safe_x && rand_y != safe_y) {
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
            bombs++;
        }
    } while (bombs < BOMBNUM);
}

void grid_deinit() {
    initialized = false;
    clean_grid();
}

bool grid_is_initialized() { return initialized; }

Cell *grid_uncover(int x, int y) {
    if (is_oob(x, y)) {
        panic("grid_uncover was called on a cell that was out of bounds");
        return (Cell *)0;
    }
    matrix[x][y].uncovered = true;
    if (matrix[x][y].number != 0) {
        return &matrix[x][y];
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
    return &matrix[x][y];
}

void grid_set_flagged(int x, int y, bool state) {
    if (is_oob(x, y)) {
        panic("grid_set_flagged was called on a cell that was out of bounds");
        return;
    }
    matrix[x][y].flag = state;
}

void grid_toggle_flag(int x, int y) { matrix[x][y].flag = !matrix[x][y].flag; }
