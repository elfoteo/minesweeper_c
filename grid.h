#include <stdbool.h>

#define CELL_SIZE 40
#define CELL_PADDING 10
#define BOMBNUM 10
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500
#define GRID_H (SCREEN_WIDTH / (CELL_SIZE + CELL_PADDING))
#define GRID_W (SCREEN_HEIGHT / (CELL_SIZE + CELL_PADDING))

typedef struct Cell Cell;

struct Cell {
    bool uncovered;
    bool mine;
    int number;
    bool fresh;
    bool flag;
};

extern Cell matrix[GRID_W][GRID_H];

bool is_oob(int x, int y);
struct GetValues {
    int x;
    int y;
    bool left;
    bool right;
};
struct GetValues GetMouseGridValues();
