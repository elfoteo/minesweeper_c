#include "../asset_declarations.h"
#include "../grid.h"
#include "../main.h"
#include "../tile_renderer.h"
#include "screens.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#define ELEMENT_PADDING 5

static bool init = false;
static Texture2D watch_texture = {0};
static const float BANNER_HEIGHT = 100.0f;

typedef struct Vector2i CellPos;
// Like Vector2 but for integers
struct Vector2i {
    int x;
    int y;
};

static CellPos mouse_to_grid(Vector2 mouse_pos) {
    CellPos r;
    r.x = (int)(mouse_pos.x / (CELL_SIZE + CELL_PADDING));
    r.y = (int)(mouse_pos.y / (CELL_SIZE + CELL_PADDING));
    return r;
}

void screen_game_draw() {
    int cursor = 0;
    if (!init) {
        init = true;
        Image img = LoadImageFromMemory(".png", assets_watch_png, assets_watch_png_len);

        watch_texture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    tile_renderer_update();
    ClearBackground(BLACK);

    float scale = 4.0f;

    Vector2 pos = {ELEMENT_PADDING, BANNER_HEIGHT / 2 - (scale * watch_texture.height) / 2};
    int font_size = watch_texture.height * scale;
    if (!grid_is_initialized()) {
        static char *str = "Click to start";
        DrawText(str, SCREEN_WIDTH / 2 - MeasureText(str, font_size) / 2, pos.y, font_size, WHITE);
    } else {
        DrawTextureEx(watch_texture, pos, 0.0f, scale, WHITE);
        pos.x += watch_texture.width * scale + ELEMENT_PADDING;
        DrawText("12:34", pos.x, pos.y, font_size, WHITE);

        // Now walk from right to left for the flags : )
        pos.x = SCREEN_WIDTH - tile_renderer_get_current_flag()->width * scale - ELEMENT_PADDING;
        tile_renderer_draw_flag(pos.x, pos.y, scale);

        static int FLAGS_PLACED = 11;
        static int FLAGS_TOTAL = 12;
        static char flags_string_buf[32];
        snprintf(flags_string_buf, 32, "%d/%d", FLAGS_PLACED, FLAGS_TOTAL);
        pos.x -= MeasureText(flags_string_buf, font_size);
        DrawText(flags_string_buf, pos.x, pos.y, font_size, WHITE);
    }

    cursor += 100;

    CellPos mouse_pos = mouse_to_grid((Vector2){GetMouseX(), GetMouseY() - cursor});

    Cell hovered_cell = matrix[mouse_pos.x][mouse_pos.y];
    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            Cell current = matrix[x][y];
            Tile tile;
            tile.cell = &current;
            tile.hovered = mouse_pos.x == x && mouse_pos.y == y;

            int cell_x = x * (CELL_SIZE + CELL_PADDING);
            int cell_y = y * (CELL_SIZE + CELL_PADDING) + cursor;
            tile_renderer_draw(&tile, cell_x + CELL_SIZE / 2, cell_y + CELL_SIZE / 2, CELL_SIZE, 0.0f);
        }
    }

    if (!hovered_cell.flag && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!grid_is_initialized()) {
            grid_init(mouse_pos.x, mouse_pos.y);
        }
        Cell *uncovered = grid_uncover(mouse_pos.x, mouse_pos.y);
        // Check if its not a nullptr
        if (uncovered != 0) {
            if (uncovered->mine) {
                game_state = STATE_LOSE;
                grid_deinit();
            }
        }
    }
    if (!hovered_cell.uncovered && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && grid_is_initialized()) {
        grid_toggle_flag(mouse_pos.x, mouse_pos.y);
    }
    if (game_state != STATE_LOSE) {
        game_state = STATE_WIN;
        // TODO: This is highly inefficent, it should be cached by the grid when a cell is uncovered
        for (int x = 0; x < GRID_W; x++) {
            for (int y = 0; y < GRID_H; y++) {
                if (!matrix[x][y].uncovered && !matrix[x][y].mine)
                    game_state = STATE_PLAYING;
            }
        }
        if (game_state == STATE_WIN) {
            grid_deinit();
        }
    }
}
