#include "../button.h"
#include "../main.h"
#include "screens.h"
#include <math.h>
#include <raylib.h>

#define NUM_MODES 4
#define CUSTOM_INDEX 3

// Presets
const GridSettings EASY = {.name = "Easy", .grid_size = 7, .mines = 5};
const GridSettings NORMAL = {.name = "Normal", .grid_size = 10, .mines = 12};
const GridSettings HARD = {.name = "Hard", .grid_size = 14, .mines = 26};
const GridSettings CUSTOM = {.name = "Custom", .grid_size = 10, .mines = 12};

// Array so that the custom mode can be edited
static GridSettings modes[NUM_MODES] = {
    EASY,
    NORMAL,
    HARD,
    CUSTOM,
};

static int current_mode = 0;

// Font stuff
const float FONT_SIZE = 60.0f;
const float FONT_SIZE_DETAILS = 20.0f;
const float FONT_SIZE_ARROWS = 80.0f;

// Animation stuff
static bool animating = false;
static float anim_offset = 0.0f;        // current offset in pixels
static float anim_target_offset = 0.0f; // target offset in pixels (+SCREEN_WIDTH or -SCREEN_WIDTH)
static int anim_dir = 0;                // +1 = moving right (prev), -1 = moving left (next)
static int anim_target_mode = 0;

static Rectangle left_rect = {0};
static Rectangle right_rect = {0};

static bool edit_mode = false;

// Helpers
static int wrap_index(int i) {
    if (i < 0)
        return (i % NUM_MODES + NUM_MODES) % NUM_MODES;
    return i % NUM_MODES;
}

static void start_animation(int dir) {
    // If something is already animating, finish it instantly
    if (animating) {
        current_mode = anim_target_mode;
        anim_offset = 0.0f;
        anim_target_offset = 0.0f;
        anim_dir = 0;
        animating = false;
    }

    anim_dir = dir;
    anim_target_offset = (float)anim_dir * (float)SCREEN_WIDTH;
    anim_offset = 0.0f;
    anim_target_mode = wrap_index(current_mode - anim_dir);
    animating = true;
}

static float lerp(float a, float b, float t) { return a + (b - a) * t; }

static int clamp_i(int v, int lo, int hi) {
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static void screen_difficulty_draw_custom_edit(GridSettings *c) {
    float cx = (float)SCREEN_WIDTH / 2.0f;
    float cy = (float)SCREEN_HEIGHT / 2.0f;

    int arrow_size = FONT_SIZE_ARROWS;
    int arrow_w = MeasureText("<", arrow_size);
    int label_size = FONT_SIZE_DETAILS * 2;

    const char *grid_text = TextFormat("Grid Size: %d", c->grid_size);
    int grid_w = MeasureText(grid_text, label_size);

    const char *mines_text = TextFormat("Mines: %d", c->mines);
    int mines_w = MeasureText(mines_text, label_size);

    float grid_y = cy - 120.0f;
    float mines_y = cy - 40.0f;

    // Arrows placed at sides, vertically aligned to center of label
    Rectangle grid_left = {(float)SCREEN_WIDTH / 5.0f - arrow_w / 2.0f - 8.0f, grid_y + (label_size - arrow_size) / 2.0f,
                           (float)arrow_w + 16.0f, (float)arrow_size + 12.0f};
    Rectangle grid_right = {(float)SCREEN_WIDTH / 5.0f * 4.0f - arrow_w / 2.0f - 8.0f, grid_y + (label_size - arrow_size) / 2.0f,
                            (float)arrow_w + 16.0f, (float)arrow_size + 12.0f};

    Rectangle mines_up = {(float)SCREEN_WIDTH / 5.0f - arrow_w / 2.0f - 8.0f, mines_y + (label_size - arrow_size) / 2.0f,
                          (float)arrow_w + 16.0f, (float)arrow_size + 12.0f};
    Rectangle mines_down = {(float)SCREEN_WIDTH / 5.0f * 4.0f - arrow_w / 2.0f - 8.0f, mines_y + (label_size - arrow_size) / 2.0f,
                            (float)arrow_w + 16.0f, (float)arrow_size + 12.0f};

    Vector2 mouse = GetMousePosition();
    bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (IsKeyPressed(KEY_LEFT))
        c->grid_size--;
    if (IsKeyPressed(KEY_RIGHT))
        c->grid_size++;
    if (IsKeyPressed(KEY_UP))
        c->mines++;
    if (IsKeyPressed(KEY_DOWN))
        c->mines--;

    if (click) {
        if (CheckCollisionPointRec(mouse, grid_left))
            c->grid_size--;
        else if (CheckCollisionPointRec(mouse, grid_right))
            c->grid_size++;
        else if (CheckCollisionPointRec(mouse, mines_up))
            c->mines--;
        else if (CheckCollisionPointRec(mouse, mines_down))
            c->mines++;
    }

    c->grid_size = clamp_i(c->grid_size, 3, 20);
    int max_mines = c->grid_size * c->grid_size - 1;
    c->mines = clamp_i(c->mines, 1, max_mines);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        edit_mode = false;

    ClearBackground(BLACK);

    const char *title = "Edit";
    int tw = MeasureText(title, FONT_SIZE);
    DrawText(title, (int)(cx - tw / 2.0f), 80 - FONT_SIZE / 2, FONT_SIZE, WHITE);

    // Grid Size row
    DrawText("<", (int)(grid_left.x + 8), (int)(grid_left.y), arrow_size, WHITE);
    DrawText(grid_text, (int)(cx - grid_w / 2.0f), (int)grid_y, label_size, WHITE);
    DrawText(">", (int)(grid_right.x + 8), (int)(grid_right.y), arrow_size, WHITE);

    // Mines row with rotated arrows
    const char *arrow_char = "<";
    float label_center_y = mines_y + label_size / 2.0f;

    // up arrow (rotated -90°)
    float up_center_x = mines_up.x + mines_up.width / 2.0f;
    Vector2 pos_up = {up_center_x, label_center_y};
    Vector2 origin_up = {(float)arrow_w / 2.0f, (float)arrow_size / 2.0f};
    DrawTextPro(GetFontDefault(), arrow_char, pos_up, origin_up, -90.0f, (float)arrow_size, 0.0f, WHITE);

    // label
    DrawText(mines_text, (int)(cx - mines_w / 2.0f), (int)(mines_y), label_size, WHITE);

    // down arrow (rotated 90°)
    float down_center_x = mines_down.x + mines_down.width / 2.0f;
    Vector2 pos_down = {down_center_x, label_center_y};
    Vector2 origin_down = origin_up;
    DrawTextPro(GetFontDefault(), arrow_char, pos_down, origin_down, 90.0f, (float)arrow_size, 0.0f, WHITE);

    // Instruction hints
    const char *hint1 = "Left / Right : change grid size";
    const char *hint2 = "Up   / Down  : change mines";
    const char *hint3 = "Enter : confirm";

    DrawText(hint1, (int)(cx - MeasureText(hint1, FONT_SIZE_DETAILS) / 2.0f), (int)(cy + 90), FONT_SIZE_DETAILS, Fade(WHITE, 0.9f));
    DrawText(hint2, (int)(cx - MeasureText(hint2, FONT_SIZE_DETAILS) / 2.0f), (int)(cy + 90 + FONT_SIZE_DETAILS * 1.2f), FONT_SIZE_DETAILS,
             Fade(WHITE, 0.9f));
    DrawText(hint3, (int)(cx - MeasureText(hint3, FONT_SIZE_DETAILS) / 2.0f), (int)(cy + 90 + FONT_SIZE_DETAILS * 2.4f), FONT_SIZE_DETAILS,
             Fade(WHITE, 0.9f));

    const char *btn_hint = "Click arrows to change values";
    DrawText(btn_hint, (int)(cx - MeasureText(btn_hint, FONT_SIZE_DETAILS) / 2.0f), (int)(cy + 140 + FONT_SIZE_DETAILS), FONT_SIZE_DETAILS,
             Fade(WHITE, 0.8f));
}

GridSettings *screen_difficulty_draw() {
    // Update input & animation
    Vector2 mouse = GetMousePosition();

    // Define arrow rectangles (large clickable area)
    int arrow_text_w = MeasureText("<", (int)FONT_SIZE_ARROWS);
    int arrow_text_h = (int)FONT_SIZE_ARROWS;
    float arrow_y = (float)SCREEN_HEIGHT / 2.0f - FONT_SIZE_ARROWS / 2.0f;
    left_rect.x = (float)SCREEN_WIDTH / 5.0f - arrow_text_w * 0.5f - 8.0f;
    left_rect.y = arrow_y - 8.0f;
    left_rect.width = arrow_text_w + 16.0f;
    left_rect.height = (float)arrow_text_h + 16.0f;

    right_rect.x = (float)SCREEN_WIDTH / 5.0f * 4.0f - arrow_text_w * 0.5f - 8.0f;
    right_rect.y = arrow_y - 8.0f;
    right_rect.width = arrow_text_w + 16.0f;
    right_rect.height = (float)arrow_text_h + 16.0f;

    bool hover_left = CheckCollisionPointRec(mouse, left_rect);
    bool hover_right = CheckCollisionPointRec(mouse, right_rect);

    if (edit_mode) {
        screen_difficulty_draw_custom_edit(&modes[CUSTOM_INDEX]);
        return (void *)0;
    }

    // Navigation when not editing
    if (IsKeyPressed(KEY_RIGHT))
        start_animation(-1);
    if (IsKeyPressed(KEY_LEFT))
        start_animation(+1);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hover_left)
            start_animation(+1);
        if (hover_right)
            start_animation(-1);
    }

    if (!animating && current_mode == CUSTOM_INDEX && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) {
        edit_mode = true;
    }

    // animation step
    if (animating) {
        anim_offset = lerp(anim_offset, anim_target_offset, 0.16f);

        if (fabsf(anim_target_offset - anim_offset) < 0.5f) {
            current_mode = anim_target_mode;
            anim_offset = 0.0f;
            anim_target_offset = 0.0f;
            anim_dir = 0;
            animating = false;
        }
    }

    // Draw background / title
    ClearBackground(BLACK);
    const char *title_str = "New Game";
    int title_w = MeasureText(title_str, (int)FONT_SIZE);
    DrawText(title_str, SCREEN_WIDTH / 2 - title_w / 2, 80 - (int)(FONT_SIZE / 2.0f), (int)FONT_SIZE, WHITE);

    int prev_idx = wrap_index(current_mode - 1);
    int next_idx = wrap_index(current_mode + 1);

    float center_x = (float)SCREEN_WIDTH / 2.0f;
    float center_y = (float)SCREEN_HEIGHT / 2.0f;

    if (animating) {
        const GridSettings *origin = &modes[current_mode];
        const GridSettings *target = &modes[anim_target_mode];

        float origin_x = center_x + anim_offset;
        float origin_y = center_y;

        float target_x = center_x + anim_offset - (float)anim_dir * (float)SCREEN_WIDTH;
        float target_y = center_y;

        float origin_fade = 0.9f;
        float origin_scale = 0.98f;
        int origin_fs = (int)roundf(FONT_SIZE * origin_scale);
        int origin_w = MeasureText(origin->name, origin_fs);
        DrawText(origin->name, (int)(origin_x - origin_w / 2.0f), (int)(origin_y - FONT_SIZE * origin_scale / 2.0f), origin_fs,
                 Fade(WHITE, origin_fade));

        const char *grid_text = TextFormat("Grid Size: %dx%d", origin->grid_size, origin->grid_size);
        DrawText(grid_text, (int)(origin_x - MeasureText(grid_text, (int)FONT_SIZE_DETAILS) / 2.0f),
                 (int)(origin_y - FONT_SIZE_DETAILS / 2.0f + 60), (int)FONT_SIZE_DETAILS, Fade(WHITE, 0.55f));

        const char *mines_text = TextFormat("Mines: %d", origin->mines);
        DrawText(mines_text, (int)(origin_x - MeasureText(mines_text, (int)FONT_SIZE_DETAILS) / 2.0f),
                 (int)(origin_y - FONT_SIZE_DETAILS / 2.0f + 60 + FONT_SIZE_DETAILS * 1.5f), (int)FONT_SIZE_DETAILS, Fade(WHITE, 0.55f));

        float target_scale = 1.02f;
        int target_fs = (int)roundf(FONT_SIZE * target_scale);
        int target_w = MeasureText(target->name, target_fs);
        DrawText(target->name, (int)(target_x - target_w / 2.0f), (int)(target_y - FONT_SIZE * target_scale / 2.0f), target_fs, WHITE);

        grid_text = TextFormat("Grid Size: %dx%d", target->grid_size, target->grid_size);
        DrawText(grid_text, (int)(target_x - MeasureText(grid_text, (int)FONT_SIZE_DETAILS) / 2.0f),
                 (int)(target_y - FONT_SIZE_DETAILS / 2.0f + 60), (int)FONT_SIZE_DETAILS, WHITE);

        mines_text = TextFormat("Mines: %d", target->mines);
        DrawText(mines_text, (int)(target_x - MeasureText(mines_text, (int)FONT_SIZE_DETAILS) / 2.0f),
                 (int)(target_y - FONT_SIZE_DETAILS / 2.0f + 60 + FONT_SIZE_DETAILS * 1.5f), (int)FONT_SIZE_DETAILS, WHITE);

    } else {
        const GridSettings *selected = &modes[current_mode];

        float center_scale = 1.0f;

        int name_fs = (int)roundf(FONT_SIZE * center_scale);
        int name_w = MeasureText(selected->name, name_fs);
        DrawText(selected->name, (int)(center_x - name_w / 2.0f), (int)(center_y - FONT_SIZE * center_scale / 2.0f), name_fs, WHITE);

        int details_fs = (int)roundf(FONT_SIZE_DETAILS);

        const char *grid_text = TextFormat("Grid Size: %dx%d", selected->grid_size, selected->grid_size);
        int details_w = MeasureText(grid_text, details_fs);
        DrawText(grid_text, (int)(center_x - details_w / 2.0f), (int)(center_y - FONT_SIZE_DETAILS / 2.0f + 60), details_fs, WHITE);

        const char *mines_text = TextFormat("Mines: %d", selected->mines);
        details_w = MeasureText(mines_text, details_fs);
        DrawText(mines_text, (int)(center_x - details_w / 2.0f), (int)(center_y - FONT_SIZE_DETAILS / 2.0f + 60 + FONT_SIZE_DETAILS * 1.5f),
                 details_fs, WHITE);
    }

    // Draw arrows
    int left_w = MeasureText("<", FONT_SIZE_ARROWS);
    int right_w = MeasureText(">", FONT_SIZE_ARROWS);

    float left_x = (float)SCREEN_WIDTH / 5.0f - left_w / 2.0f;
    float right_x = (float)SCREEN_WIDTH / 5.0f * 4.0f - right_w / 2.0f;
    float arrow_draw_y = arrow_y;

    DrawText("<", (int)left_x, (int)arrow_draw_y, FONT_SIZE_ARROWS, WHITE);
    DrawText(">", (int)right_x, (int)arrow_draw_y, FONT_SIZE_ARROWS, WHITE);

    if (current_mode == CUSTOM_INDEX || anim_target_mode == CUSTOM_INDEX) {
        const char *hint = "Enter to edit";
        int hw = MeasureText(hint, (int)FONT_SIZE_DETAILS);

        float hint_x = 0.0f;

        if (animating) {
            // Determine where the CUSTOM_INDEX item is currently
            if (anim_target_mode == CUSTOM_INDEX) {
                // incoming item
                hint_x = center_x + anim_offset - (float)anim_dir * (float)SCREEN_WIDTH;
            } else {
                // outgoing origin
                hint_x = center_x + anim_offset;
            }
        } else {
            hint_x = center_x;
        }

        DrawText(hint, (int)(hint_x - hw / 2.0f), SCREEN_HEIGHT - 190, (int)FONT_SIZE_DETAILS, Fade(WHITE, 0.75f));
    }

    if (button_draw_centered("Play", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80, 200, 60, BUTTON_DEFAULT_STYLE)) {
        return &modes[current_mode];
    }

    return (void *)0;
}
