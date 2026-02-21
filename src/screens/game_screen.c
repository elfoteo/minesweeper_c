#include "../asset_declarations.h"
#include "../grid.h"
#include "../main.h"
#include "screens.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

#define ELEMENT_PADDING 5
#define FLAG_FRAMES_COUNT 13 + 1 // Zero indexed, total of 14 frames
#define FLAG_FRAME_TIME 0.15     // in seconds

static bool init = false;
static Texture2D watch_texture = {0};
static Texture2D flag_textures[FLAG_FRAMES_COUNT] = {0};
static const float BANNER_HEIGHT = 100.0f;
static int flag_current_frame = 0;
static float timer = 0;

typedef struct Vector2i CellPos;
typedef struct Tile Tile;
struct Tile {
    Cell *cell;
    bool hovered;
};
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

static void tile_draw(Tile *tile, int x, int y) {
    Color squarecolour = RED;

    if (tile->hovered) {
        float d = 0.65;
        squarecolour = (Color){230 * d, 41 * d, 55 * d, 255};
    }

    if (tile->cell->flag) {
        squarecolour = tile->hovered ? ORANGE : YELLOW;
    } else if (tile->cell->uncovered) {
        if (tile->cell->mine)
            squarecolour = BLACK;
        else
            squarecolour = tile->hovered ? DARKBLUE : BLUE;
    }

#if XRAY
    if (!current.uncovered)
        squarecolour = DARKPURPLE;
#endif
    // Gray dropshadow
    DrawRectangle(x + 4, y + 4, CELL_SIZE, CELL_SIZE, GRAY);
    // Draw the tile
    DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, squarecolour);
    if (tile->cell->uncovered || XRAY) {
        // Assume its a mine, if its a number correct it later
        const char *number = "*";

        if (!tile->cell->mine) {
            number = TextFormat("%i", tile->cell->number);
        }

        DrawText(number, x + 12, y + 7, 30, WHITE);
    } else if (tile->cell->flag) {
        Vector2 flag_pos = {x - 4, y - 4};
        DrawTextureEx(flag_textures[flag_current_frame], flag_pos, 0.0f, 3, WHITE);
    }
}

void screen_game_draw() {
    int cursor = 0;
    if (!init) {
        init = true;
        Image img = LoadImageFromMemory(".png", assets_watch_png, assets_watch_png_len);

        watch_texture = LoadTextureFromImage(img);
        UnloadImage(img);

        // Flags, I want to KMS
        static unsigned int flag_png_sizes[FLAG_FRAMES_COUNT];
        static unsigned char *flag_png_data[FLAG_FRAMES_COUNT];
        flag_png_data[0] = assets_flag_0_png;
        flag_png_data[1] = assets_flag_1_png;
        flag_png_data[2] = assets_flag_2_png;
        flag_png_data[3] = assets_flag_3_png;
        flag_png_data[4] = assets_flag_4_png;
        flag_png_data[5] = assets_flag_5_png;
        flag_png_data[6] = assets_flag_6_png;
        flag_png_data[7] = assets_flag_7_png;
        flag_png_data[8] = assets_flag_8_png;
        flag_png_data[9] = assets_flag_9_png;
        flag_png_data[10] = assets_flag_10_png;
        flag_png_data[11] = assets_flag_11_png;
        flag_png_data[12] = assets_flag_12_png;
        flag_png_data[13] = assets_flag_13_png;

        flag_png_sizes[0] = assets_flag_0_png_len;
        flag_png_sizes[1] = assets_flag_1_png_len;
        flag_png_sizes[2] = assets_flag_2_png_len;
        flag_png_sizes[3] = assets_flag_3_png_len;
        flag_png_sizes[4] = assets_flag_4_png_len;
        flag_png_sizes[5] = assets_flag_5_png_len;
        flag_png_sizes[6] = assets_flag_6_png_len;
        flag_png_sizes[7] = assets_flag_7_png_len;
        flag_png_sizes[8] = assets_flag_8_png_len;
        flag_png_sizes[9] = assets_flag_9_png_len;
        flag_png_sizes[10] = assets_flag_10_png_len;
        flag_png_sizes[11] = assets_flag_11_png_len;
        flag_png_sizes[12] = assets_flag_12_png_len;
        flag_png_sizes[13] = assets_flag_13_png_len;

        for (int i = 0; i < FLAG_FRAMES_COUNT; i++) {
            Image frame = LoadImageFromMemory(".png", flag_png_data[i], flag_png_sizes[i]);
            flag_textures[i] = LoadTextureFromImage(frame);
            UnloadImage(frame);
        }
    }

    // Increment current flag frame
    timer += GetFrameTime();
    while (timer >= 0.15f) // if the computer is laggy asf it will skip more then 150ms per frame
    {
        flag_current_frame = (flag_current_frame + 1) % FLAG_FRAMES_COUNT;
        timer -= 0.15f;
    }

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
        pos.x = SCREEN_WIDTH - flag_textures[flag_current_frame].width * scale - ELEMENT_PADDING;
        DrawTextureEx(flag_textures[flag_current_frame], pos, 0.0f, scale, WHITE);

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
            tile_draw(&tile, cell_x, cell_y);
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
