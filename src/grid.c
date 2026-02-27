#include "grid.h"
#include "raylib.h"
#include "utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
Cell **matrix;
gridval state;
bool initialized = false;

bool is_oob(int x, int y) { return x < 0 || x > state.grid_w - 1 || y < 0 || y > state.grid_h - 1; }

static void clean_grid() {
    for (int x = 0; x < state.grid_w; x++) {
        for (int y = 0; y < state.grid_h; y++) {
            matrix[x][y].mine = 0;
            matrix[x][y].uncovered = 0;
            matrix[x][y].number = 0;
            matrix[x][y].flag = 0;
        }
    }
}

void grid_init(int safe_x, int safe_y, int grid_h, int grid_w, int bombnum) {
    srand(time(NULL));
    if (initialized) {
        panic("Grid already initialized\n");
    }

    if (bombnum > grid_w * grid_h - 1) {
        panic("Too many bombs for the grid size!\n");
    }
    initialized = true;
    matrix = malloc(grid_h * sizeof(Cell *));
    if (matrix == NULL) {
        panic("Buy more ram lol\n");
    }
    for (int i = 0; i < grid_h; i++) {
        matrix[i] = malloc(grid_w * sizeof(Cell));
        if (matrix[i] == NULL) {
            panic("Buy more ram lol\n");
        }
    }
    state.grid_w = grid_w;
    state.grid_h = grid_h;
    state.bombnum = bombnum;
    clean_grid();

    int bombs = 0;
    do {
        int rand_x = rand() % grid_w;
        int rand_y = rand() % grid_h;
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
    } while (bombs < bombnum);
}

void grid_deinit() {
    initialized = false;
    if (matrix != 0) {
        free(matrix);
        matrix = 0;
    }
}

bool grid_is_initialized() { return initialized; }

Cell *grid_uncover(int x, int y) {
    if (is_oob(x, y)) {
        panic("grid_uncover was called on a cell that was out of bounds");
        return (Cell *)0;
    }
    matrix[x][y].uncovered = true;
    matrix[x][y].flag = false;
    if (matrix[x][y].number != 0 || matrix[x][y].mine) {
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

bool grid_is_flagged(int x, int y) {
    if (is_oob(x, y)) {
        panic("grid_is_flagged was called on a cell that was out of bounds");
        return false;
    }
    return matrix[x][y].flag;
}

void grid_toggle_flag(int x, int y) {
    if (matrix[x][y].uncovered) {
        return;
    }
    matrix[x][y].flag = !matrix[x][y].flag;
}
