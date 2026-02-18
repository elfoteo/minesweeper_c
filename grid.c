#include "grid.h"
#include "raylib.h"
#include <stdbool.h>
bool is_oob(int x, int y) { return x < 0 || x > GRID_W - 1 || y < 0 || y > GRID_H - 1; }

struct GetValues {
    int x;
    int y;
    bool left;
    bool right;
};
struct GetValues GetMouseGridValues() {
    struct GetValues r;
    r.x = GetMouseX() / (CELL_SIZE + CELL_PADDING);
    r.y = GetMouseY() / (CELL_SIZE + CELL_PADDING);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        r.left = true;
    else
        r.left = false;
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        r.right = true;
    else
        r.right = false;
    return r;
}
