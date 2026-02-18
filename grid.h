#include <stdbool.h>

#define GRID_H 10
#define GRID_W 10

typedef struct Cell Cell;

struct Cell {
    bool uncovered;
    bool mine;
    int number;
    bool fresh;
    bool flag;
};

extern Cell matrix[GRID_W][GRID_H];

// Tries to uncover a flag, if out of bounds should print an error to the console and safely exit from app
// It should automatically try and uncover adjacent cells if the number of the uncovered cells equals 0
void grid_uncover(int x, int y);

// Tries to set a cell of the grid as flagged, if out of bounds should print an error to the console and safely exit from app
void grid_flag(int x, int y);

// Returns the struct for the Cell at a given coord, if out of bounds should print an error to the console and safely exit from app
Cell grid_get_cell(int x, int y);
