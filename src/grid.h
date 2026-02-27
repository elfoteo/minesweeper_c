#ifndef GRID_HH
#define GRID_HH

#include "main.h"
#include <stdbool.h>

#define CELL_SIZE 40
#define CELL_PADDING 10

typedef struct gridval gridval;
struct gridval {
    int grid_w;
    int grid_h;
    int bombnum;
};

extern gridval state;

typedef struct Cell Cell;

struct Cell {
    bool uncovered;
    bool mine;
    int number;
    bool flag;
};

extern Cell **matrix;

bool is_oob(int x, int y);

void grid_init(int safe_x, int safe_y, int grid_h, int grid_w, int bombnum);
void grid_deinit();
bool grid_is_initialized();

Cell *grid_uncover(int x, int y);
void grid_flag(int x, int y, bool state);
void grid_toggle_flag(int x, int y);
bool grid_is_flagged(int x, int y);

#endif // GRID_HH
