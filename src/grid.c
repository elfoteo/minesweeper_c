#include "grid.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Cell **matrix;
gridval grid_state;
bool initialized = false;

bool is_oob(int x, int y) { return x < 0 || x > grid_state.grid_w - 1 || y < 0 || y > grid_state.grid_h - 1; }

static void clean_grid() {
    if (matrix == 0) {
        panic("clean_grid: matrix was a nullptr\n");
        return;
    }
    for (int x = 0; x < grid_state.grid_w; x++) {
        for (int y = 0; y < grid_state.grid_h; y++) {
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

    // Ensure we have enough space for the safe zone (5x5 around safe_x, safe_y)
    // and still have room for mines.
    if (bombnum > grid_w * grid_h - 25) {
        // If the grid is very small, we might not be able to guarantee 5x5
        // but let's assume the UI prevents this for now.
    }

    if (bombnum >= grid_w * grid_h) {
        panic("Too many bombs for the grid size!\n");
    }

    initialized = true;
    matrix = malloc(grid_w * sizeof(Cell *));
    if (matrix == NULL) {
        panic("Buy more ram lol\n");
    }
    for (int i = 0; i < grid_w; i++) {
        matrix[i] = malloc(grid_h * sizeof(Cell));
        if (matrix[i] == NULL) {
            panic("Buy more ram lol\n");
        }
    }
    grid_state.grid_w = grid_w;
    grid_state.grid_h = grid_h;
    grid_state.bombnum = bombnum;
    grid_state.flags = bombnum;
    grid_state.uncovered = 0;
    clean_grid();

    int bombs = 0;
    int attempts = 0;
    const int MAX_ATTEMPTS = grid_w * grid_h * 10;

    while (bombs < bombnum && attempts < MAX_ATTEMPTS) {
        attempts++;
        int rand_x = rand() % grid_w;
        int rand_y = rand() % grid_h;

        // Skip if mine already exists
        if (matrix[rand_x][rand_y].mine) continue;

        // Safe zone: no mines in the 5x5 area centered at (safe_x, safe_y)
        // This ensures (safe_x, safe_y) and its 3x3 neighbors have number 0.
        bool in_safe_zone = (rand_x >= safe_x - 2 && rand_x <= safe_x + 2 &&
                             rand_y >= safe_y - 2 && rand_y <= safe_y + 2);
        
        if (in_safe_zone) continue;

        matrix[rand_x][rand_y].mine = true;
        bombs++;

        // Update numbers for neighbors
        for (int x = -1; x < 2; x++) {
            for (int y = -1; y < 2; y++) {
                int ax = rand_x + x;
                int ay = rand_y + y;
                if (is_oob(ax, ay) || (x == 0 && y == 0)) {
                    continue;
                }
                matrix[ax][ay].number++;
            }
        }
    }

    if (bombs < bombnum) {
        // Fallback: if we couldn't place all bombs in the non-safe zone,
        // just place them anywhere that isn't the safe_x, safe_y itself
        // though with current requirements this shouldn't happen for reasonable grids.
        for (int x = 0; x < grid_w && bombs < bombnum; x++) {
            for (int y = 0; y < grid_h && bombs < bombnum; y++) {
                if (!matrix[x][y].mine && (x != safe_x || y != safe_y)) {
                    // Check if it's NOT in the immediate 3x3 to at least guarantee safe click
                    if (x >= safe_x - 1 && x <= safe_x + 1 && y >= safe_y - 1 && y <= safe_y + 1) continue;
                    
                    matrix[x][y].mine = true;
                    bombs++;
                    for (int dx = -1; dx < 2; dx++) {
                        for (int dy = -1; dy < 2; dy++) {
                            int ax = x + dx;
                            int ay = y + dy;
                            if (!is_oob(ax, ay) && !(dx == 0 && dy == 0)) {
                                matrix[ax][ay].number++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void grid_deinit() {
    initialized = false;
    if (matrix != 0) {
        for (int i = 0; i < grid_state.grid_w; i++) {
            free(matrix[i]);
        }
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
    if (matrix[x][y].uncovered) return &matrix[x][y];

    matrix[x][y].uncovered = true;
    matrix[x][y].flag = false;
    grid_state.uncovered++;

    if (matrix[x][y].mine) {
        return &matrix[x][y];
    }

    if (matrix[x][y].number == 0) {
        // Check 3x3 neighbours excluding the center and uncover them
        for (int x0 = -1; x0 < 2; x0++) {
            for (int y0 = -1; y0 < 2; y0++) {
                if (x0 == 0 && y0 == 0) continue;
                int ax = x + x0;
                int ay = y + y0;
                if (!is_oob(ax, ay)) {
                    if (!matrix[ax][ay].uncovered) {
                        grid_uncover(ax, ay);
                    }
                }
            }
        }
    }
    return &matrix[x][y];
}

void grid_uncover_all_mines() {
    if (!initialized) {
        panic("grid_uncover_all_mines was called but the gird wasn't even initialized\n");
        return;
    }
    for (int x = 0; x < grid_state.grid_w; x++) {
        for (int y = 0; y < grid_state.grid_h; y++) {
            if (matrix[x][y].mine) {
                matrix[x][y].uncovered = true;
            }
        }
    }
}

void grid_set_flagged(int x, int y, bool state) {
    if (!initialized) {
        panic("grid_set_flagged was called but the gird wasn't even initialized\n");
        return;
    }
    if (is_oob(x, y)) {
        panic("grid_set_flagged was called on a cell that was out of bounds\n");
        return;
    }
    if (matrix[x][y].uncovered) return;
    
    if (matrix[x][y].flag == state) return;

    matrix[x][y].flag = state;
    if (state) {
        grid_state.flags--;
    } else {
        grid_state.flags++;
    }
}

bool grid_is_flagged(int x, int y) {
    if (!initialized) {
        panic("grid_is_flagged was called but the gird wasn't even initialized\n");
        return false;
    }
    if (is_oob(x, y)) {
        panic("grid_is_flagged was called on a cell that was out of bounds\n");
        return false;
    }
    return matrix[x][y].flag;
}

void grid_toggle_flag(int x, int y) {
    if (!initialized) {
        panic("grid_toggle_flagged was called but the gird wasn't even initialized\n");
        return;
    }
    if (is_oob(x, y)) {
        panic("grid_toggle_flag was called on a cell that was out of bounds\n");
        return;
    }
    if (matrix[x][y].uncovered) {
        return;
    }
    grid_set_flagged(x, y, !matrix[x][y].flag);
}
