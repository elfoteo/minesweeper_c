#include "tile_renderer.h"
#include "asset_declarations.h"
#include "utils.h"
#include <raylib.h>
#include <stdbool.h>

#define FLAG_FRAMES_COUNT 13 + 1 // Zero indexed, total of 14 frames
#define FLAG_FRAME_TIME 0.15     // in seconds

static bool initialized = false;
static Texture2D flag_textures[FLAG_FRAMES_COUNT] = {0};
static int flag_current_frame = 0;
static float timer = 0;
static Font font = {0};
static const float FONT_SPACING = 1.0f;

void tile_renderer_init() {
    if (initialized) {
        panic("Tried to initialize the tile renderer when it was already initialized");
        return;
    }

    font = GetFontDefault();

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

void tile_renderer_update() {
    // Increment current flag frame
    timer += GetFrameTime();
    while (timer >= 0.15f) // if the computer is laggy asf it will skip more then 150ms per frame
    {
        flag_current_frame = (flag_current_frame + 1) % FLAG_FRAMES_COUNT;
        timer -= 0.15f;
    }
}

void tile_renderer_draw_flag(int x, int y, float scale) {
    DrawTextureEx(flag_textures[flag_current_frame], (Vector2){x, y}, 0.0f, scale, WHITE);
}

Texture2D *tile_renderer_get_current_flag() { return &flag_textures[flag_current_frame]; }

void tile_renderer_draw(Tile *tile, int x, int y, float size, float rot) {
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
    DrawRectanglePro((Rectangle){x + 4, y + 4, size, size}, (Vector2){size / 2, size / 2}, rot, GRAY);
    // Draw the tile
    DrawRectanglePro((Rectangle){x, y, size, size}, (Vector2){size / 2, size / 2}, rot, squarecolour);
    if (tile->cell->uncovered || XRAY) {
        // Assume its a mine, if its a number correct it later
        const char *number = "*";

        if (!tile->cell->mine) {
            number = TextFormat("%i", tile->cell->number);
        }
        float font_size = size;

        Vector2 textSize = MeasureTextEx(font, number, font_size, FONT_SPACING);

        float text_w = MeasureText(number, size);
        DrawTextPro(GetFontDefault(), number, (Vector2){x, y}, (Vector2){textSize.x / 2, textSize.y / 2}, rot, size, 1, WHITE);
    } else if (tile->cell->flag) {
        Vector2 flag_pos = {x - size / 2 - 4, y - size / 2 - 4};
        DrawTextureEx(flag_textures[flag_current_frame], flag_pos, 0.0f, 3, WHITE);
    }
}
