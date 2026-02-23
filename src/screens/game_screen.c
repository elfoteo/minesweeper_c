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
static const double LONG_CLICK_THRESHOLD = 1.0; // In seconds

// long-click state
static double long_click_start_time = -1.0;
static bool long_click_triggered = false;

static bool game_ended = false;

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

void screen_game_draw(GridSettings *gs) {
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
    int font_size = (int)(watch_texture.height * scale);
    if (!grid_is_initialized()) {
        static char *str = "Click to start";
        DrawText(str, SCREEN_WIDTH / 2 - MeasureText(str, font_size) / 2, (int)pos.y, font_size, WHITE);
    } else if (game_ended) {
        static char *str = "Game Over";
        DrawText(str, SCREEN_WIDTH / 2 - MeasureText(str, font_size) / 2, (int)(pos.y - ELEMENT_PADDING * 3), font_size, WHITE);
        static char *str2 = "Click to continue";
        DrawText(str2, SCREEN_WIDTH / 2 - MeasureText(str2, 20) / 2, (int)(pos.y + font_size - ELEMENT_PADDING * 2), 20, WHITE);
    } else {
        DrawTextureEx(watch_texture, pos, 0.0f, scale, WHITE);
        pos.x += watch_texture.width * scale + ELEMENT_PADDING;
        DrawText("12:34", (int)pos.x, (int)pos.y, font_size, WHITE);

        // Now walk from right to left for the flags : )
        pos.x = SCREEN_WIDTH - tile_renderer_get_current_flag()->width * scale - ELEMENT_PADDING;
        tile_renderer_draw_flag((int)pos.x, (int)pos.y, scale);

        static int FLAGS_PLACED = 11;
        static int FLAGS_TOTAL = 12;
        static char flags_string_buf[32];
        snprintf(flags_string_buf, 32, "%d/%d", FLAGS_PLACED, FLAGS_TOTAL);
        pos.x -= MeasureText(flags_string_buf, font_size);
        DrawText(flags_string_buf, (int)pos.x, (int)pos.y, font_size, WHITE);
    }

    cursor += 100;

    // Compute mouse grid position and guard bounds BEFORE using it
    Vector2 raw_mouse = (Vector2){(float)GetMouseX(), (float)(GetMouseY() - cursor)};
    CellPos mouse_pos = mouse_to_grid(raw_mouse);

    bool mouse_in_bounds = (mouse_pos.x >= 0 && mouse_pos.x < GRID_W && mouse_pos.y >= 0 && mouse_pos.y < GRID_H);

    // Draw the grid
    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            // point tile.cell at actual matrix cell (don't take address of temporary)
            Tile tile;
            tile.cell = &matrix[x][y];
            tile.hovered = mouse_in_bounds && (mouse_pos.x == x && mouse_pos.y == y);

            int cell_x = x * (CELL_SIZE + CELL_PADDING);
            int cell_y = y * (CELL_SIZE + CELL_PADDING) + cursor;
            tile_renderer_draw(&tile, cell_x + CELL_SIZE / 2, cell_y + CELL_SIZE / 2, CELL_SIZE, 0.0f);
        }
    }

    // Update input and game logic
    if (!game_ended) {
        // Start timing on press
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            long_click_start_time = GetTime();
            long_click_triggered = false;
        }

        // If button is held and not yet triggered, check threshold
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && long_click_start_time >= 0.0 && !long_click_triggered) {
            if (GetTime() - long_click_start_time >= LONG_CLICK_THRESHOLD) {
                // Only toggle flag if mouse is over a valid cell
                if (mouse_in_bounds) {
                    grid_toggle_flag(mouse_pos.x, mouse_pos.y);
                }
                long_click_triggered = true; // prevent repeated toggles during same hold
            }
        }

        // On release if long-click didn't trigger do a normal click
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            if (!long_click_triggered) {
                if (mouse_in_bounds) {
                    // First click initializes grid
                    if (!grid_is_initialized()) {
                        grid_init(mouse_pos.x, mouse_pos.y);
                    }
                    // Prevent accidental uncover if the cell is flagged
                    if (!grid_is_flagged(mouse_pos.x, mouse_pos.y)) {
                        Cell *uncovered = grid_uncover(mouse_pos.x, mouse_pos.y);
                        if (uncovered && uncovered->mine) {
                            game_ended = true; // TODO: Uncover all the mines in the grid
                        }
                    }
                }
            }
            // Reset long-click state on release
            long_click_start_time = -1.0;
            long_click_triggered = false;
        }

        // right click flagging
        if (mouse_in_bounds && !matrix[mouse_pos.x][mouse_pos.y].uncovered && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) &&
            grid_is_initialized()) {
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
    } else {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            game_state = STATE_LOSE;
            grid_deinit();
            game_ended = false;
        }
    }
}
