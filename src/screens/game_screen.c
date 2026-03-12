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

static bool game_won = false;
static bool game_ended = false;
static double game_timer = 0.0;

typedef struct Vector2i CellPos;
// Like Vector2 but for integers
struct Vector2i {
    int x;
    int y;
};

static int get_cell_size_total(GridSettings *gs) {
    float available_width = (float)GetScreenWidth();
    float available_height = (float)GetScreenHeight() - BANNER_HEIGHT;
    float size = (available_width < available_height) ? available_width : available_height;
    return (int)(size / gs->grid_size);
}

static Vector2 get_grid_offset(GridSettings *gs) {
    float available_width = (float)GetScreenWidth();
    float available_height = (float)GetScreenHeight() - BANNER_HEIGHT;
    int cell_size_total = get_cell_size_total(gs);
    Vector2 offset;
    offset.x = (available_width - (cell_size_total * gs->grid_size)) / 2.0f;
    offset.y = BANNER_HEIGHT + (available_height - (cell_size_total * gs->grid_size)) / 2.0f;
    return offset;
}

static CellPos mouse_to_grid(Vector2 mouse_pos, GridSettings *gs) {
    int cell_size_total = get_cell_size_total(gs);
    Vector2 offset = get_grid_offset(gs);
    CellPos r;
    r.x = (int)((mouse_pos.x - offset.x) / cell_size_total);
    r.y = (int)((mouse_pos.y - offset.y) / cell_size_total);

    // Check if mouse is actually within the grid boundaries
    if (mouse_pos.x < offset.x || mouse_pos.y < offset.y) {
        r.x = -1;
        r.y = -1;
    }
    return r;
}

static void end_game(bool won) {
    game_ended = true;
    game_won = won;
    grid_uncover_all_mines();
}

void screen_game_draw(GridSettings *gs) {
    if (grid_is_initialized() && !game_ended)
        game_timer += GetFrameTime();

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
        game_timer = 0.0;
        static char *str = "Click to start";
        DrawText(str, GetScreenWidth() / 2 - MeasureText(str, font_size) / 2, (int)pos.y, font_size, WHITE);
    } else if (game_ended) {
        if (game_won) {
            static char *str = "You won";
            DrawText(str, GetScreenWidth() / 2 - MeasureText(str, font_size) / 2, (int)(pos.y - ELEMENT_PADDING * 3), font_size, YELLOW);
        } else {
            static char *str = "Game Over";
            DrawText(str, GetScreenWidth() / 2 - MeasureText(str, font_size) / 2, (int)(pos.y - ELEMENT_PADDING * 3), font_size, WHITE);
        }
        static char *str2 = "Click to continue";
        DrawText(str2, GetScreenWidth() / 2 - MeasureText(str2, 20) / 2, (int)(pos.y + font_size - ELEMENT_PADDING * 2), 20, WHITE);
    } else {
        DrawTextureEx(watch_texture, pos, 0.0f, scale, WHITE);
        pos.x += watch_texture.width * scale + ELEMENT_PADDING;
        DrawText(TextFormat("%02d:%02d", (int)game_timer / 60, (int)game_timer % 60), (int)pos.x, (int)pos.y, font_size, WHITE);

        // Now walk from right to left for the flags : )
        pos.x = GetScreenWidth() - tile_renderer_get_current_flag()->width * scale - ELEMENT_PADDING;
        tile_renderer_draw_flag((int)pos.x, (int)pos.y, scale);

        int flags_placed = grid_state.flags;
        int flags_total = gs->mines;
        static char flags_string_buf[32];
        snprintf(flags_string_buf, 32, "%d/%d", flags_placed, flags_total);
        pos.x -= MeasureText(flags_string_buf, font_size);
        DrawText(flags_string_buf, (int)pos.x, (int)pos.y, font_size, WHITE);
    }

    // Compute mouse grid position and guard bounds BEFORE using it
    Vector2 raw_mouse = (Vector2){(float)GetMouseX(), (float)GetMouseY()};
    CellPos mouse_pos = mouse_to_grid(raw_mouse, gs);

    bool mouse_in_bounds = (mouse_pos.x >= 0 && mouse_pos.x < grid_state.grid_w && mouse_pos.y >= 0 && mouse_pos.y < grid_state.grid_h);

    // Draw the grid
    Cell fake = {.flag = false, .mine = false, .number = 0, .uncovered = false};
    int cell_size_total = get_cell_size_total(gs);
    // Increased gap ratio
    int cell_padding = cell_size_total / 5;
    if (cell_padding < 2 && cell_size_total > 5)
        cell_padding = 2;
    int cell_size = cell_size_total - cell_padding;
    Vector2 offset = get_grid_offset(gs);

    for (int x = 0; x < grid_state.grid_w; x++) {
        for (int y = 0; y < grid_state.grid_h; y++) {
            // point tile.cell at actual matrix cell (don't take address of temporary)
            Tile tile;
            tile.cell = grid_is_initialized() ? &matrix[x][y] : &fake;
            tile.hovered = mouse_in_bounds && (mouse_pos.x == x && mouse_pos.y == y);

            int cell_x = (int)offset.x + x * cell_size_total;
            int cell_y = (int)offset.y + y * cell_size_total;
            tile_renderer_draw(&tile, cell_x + cell_size / 2 + cell_padding / 2, cell_y + cell_size / 2 + cell_padding / 2,
                               (float)cell_size, 0.0f);
        }
    }

    // Update input and game logic
    if (game_ended) {
        // One last click and show the user all the hidden mines
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            game_state = game_won ? STATE_WIN : STATE_LOSE;
            grid_deinit();
            game_ended = false;
        }
    } else {
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
                        grid_init(mouse_pos.x, mouse_pos.y, gs->grid_size, gs->grid_size, gs->mines);
                    }
                    // Prevent accidental uncover if the cell is flagged
                    if (!grid_is_flagged(mouse_pos.x, mouse_pos.y)) {
                        Cell *uncovered = grid_uncover(mouse_pos.x, mouse_pos.y);
                        if (uncovered && uncovered->mine) {
                            end_game(false);
                        }
                    }
                }
            }
            // Reset long-click state on release
            long_click_start_time = -1.0;
            long_click_triggered = false;
        }

        if (grid_is_initialized()) {
            // right click flagging
            if (mouse_in_bounds && !matrix[mouse_pos.x][mouse_pos.y].uncovered && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                grid_toggle_flag(mouse_pos.x, mouse_pos.y);
            }

            int cell_count = gs->grid_size * gs->grid_size - gs->mines;
            if (grid_state.uncovered >= cell_count) {
                end_game(true);
            }
        }
    }
}
