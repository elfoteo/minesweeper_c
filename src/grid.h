#ifndef GRID_HH
#define GRID_HH

#include "main.h"
#include <stdbool.h>

#define CELL_SIZE 40
#define CELL_PADDING 10
#define BOMBNUM 10
#define GRID_H 10 // (SCREEN_WIDTH / (CELL_SIZE + CELL_PADDING))
#define GRID_W 10 // (SCREEN_HEIGHT / (CELL_SIZE + CELL_PADDING))

typedef struct Cell Cell;
typedef struct CellPos CellPos;

struct Cell {
    bool uncovered;
    bool mine;
    int number;
    bool flag;
};

struct CellPos {
    int x;
    int y;
};

extern Cell matrix[GRID_W][GRID_H];

bool is_oob(int x, int y);
CellPos mouse_to_grid();

void grid_init(int safe_x, int safe_y);
void grid_deinit();
bool grid_is_initialized();

Cell *grid_uncover(int x, int y);
void grid_flag(int x, int y, bool state);
void grid_toggle_flag(int x, int y);

#endif // GRID_HH
