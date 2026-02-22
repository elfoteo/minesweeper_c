#include "grid.h"
#include <raylib.h>

typedef struct Tile Tile;
struct Tile {
    Cell *cell;
    bool hovered;
};

void tile_renderer_init();
void tile_renderer_update();
void tile_renderer_draw_flag(int x, int y, float scale);
void tile_renderer_draw(Tile *tile, int x, int y, float size, float rot);
Texture2D *tile_renderer_get_current_flag();
