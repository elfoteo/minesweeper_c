#include "../button.h"
#include "../main.h"
#include "../tile_renderer.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

#define BUTTON_COUNT 3
#define MENU_TILE_COUNT 35

static Cell CELL_RED = {.uncovered = false, .number = 0, .flag = false, .mine = false};
static Tile TILE_RED = {.hovered = false, .cell = &CELL_RED};

// The number will be modified at runtime
static Cell CELL_NUMBER = {.uncovered = true, .number = 0, .flag = false, .mine = false};
static Tile TILE_NUMBER = {.hovered = false, .cell = &CELL_NUMBER};

typedef enum { BTN_PLAY, BTN_ABOUT, BTN_EXIT } ButtonId;
typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 base_pos;
    Vector2 rel_base_pos; // Store normalized position (0.0 to 1.0)
    float size;
    float rel_size;       // Size relative to DEFAULT_SCREEN_WIDTH
    float rotation;
    Tile *tile;
    int number;
    bool active;
} MenuTile;

static MenuTile menu_tiles[MENU_TILE_COUNT];
static bool menu_tiles_initialized = false;

typedef struct {
    const char *label;
    ButtonId id;
} MenuButton;

static MenuButton buttons[] = {{"Play", BTN_PLAY}, {"About", BTN_ABOUT}, {"Exit", BTN_EXIT}};

static int random_weighted_number() {
    int r = GetRandomValue(0, 99);

    if (r < 30)
        return 0;
    if (r < 55)
        return 1;
    if (r < 75)
        return 2;
    if (r < 90)
        return 3;
    return 4;
}

static bool is_in_exclusion_zone(Vector2 p, float sw, float sh) {
    float cx = sw * 0.5f;
    float cy = sh * 0.5f; 
    float rx = sw * 0.35f; 
    float ry = sh * 0.45f;
    
    float dx = (p.x - cx) / rx;
    float dy = (p.y - cy) / ry;
    
    return (dx * dx + dy * dy) < 1.0f;
}

static void menu_tiles_init() {
    const int MAX_ATTEMPTS = 100;

    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        float base_size = 20.0f + (float)GetRandomValue(0, 15);
        menu_tiles[i].rel_size = base_size / (float)DEFAULT_SCREEN_WIDTH;
        menu_tiles[i].size = menu_tiles[i].rel_size * sw;
        menu_tiles[i].active = false;

        bool placed = false;

        // Stage 1: Try to place without overlapping exclusion zone AND other tiles
        for (int attempt = 0; attempt < MAX_ATTEMPTS && !placed; attempt++) {
            Vector2 candidate;
            candidate.x = (float)GetRandomValue(10, (int)sw - 10);
            candidate.y = (float)GetRandomValue(10, (int)sh - 10);

            if (is_in_exclusion_zone(candidate, sw, sh)) continue;

            placed = true;
            for (int j = 0; j < i; j++) {
                if (!menu_tiles[j].active) continue;
                float minDist = (menu_tiles[i].size + menu_tiles[j].size) * 0.8f;
                if (Vector2Distance(candidate, menu_tiles[j].base_pos) < minDist) {
                    placed = false;
                    break;
                }
            }

            if (placed) {
                menu_tiles[i].base_pos = candidate;
                menu_tiles[i].active = true;
            }
        }

        // Stage 2: If Stage 1 failed, try to place without overlapping exclusion zone (allow tile-tile overlap)
        if (!placed) {
            for (int attempt = 0; attempt < MAX_ATTEMPTS && !placed; attempt++) {
                Vector2 candidate;
                candidate.x = (float)GetRandomValue(10, (int)sw - 10);
                candidate.y = (float)GetRandomValue(10, (int)sh - 10);

                if (!is_in_exclusion_zone(candidate, sw, sh)) {
                    menu_tiles[i].base_pos = candidate;
                    menu_tiles[i].active = true;
                    placed = true;
                }
            }
        }

        if (menu_tiles[i].active) {
            menu_tiles[i].rel_base_pos = (Vector2){menu_tiles[i].base_pos.x / sw, menu_tiles[i].base_pos.y / sh};
            menu_tiles[i].pos = menu_tiles[i].base_pos;
            menu_tiles[i].vel = (Vector2){0, 0};
            menu_tiles[i].rotation = 0;

            if (GetRandomValue(0, 1)) {
                menu_tiles[i].tile = &TILE_NUMBER;
                menu_tiles[i].number = random_weighted_number();
            } else {
                menu_tiles[i].tile = &TILE_RED;
                menu_tiles[i].number = -1;
            }
        }
    }
}

static void menu_tiles_reposition() {
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        if (!menu_tiles[i].active) continue;
        menu_tiles[i].base_pos.x = menu_tiles[i].rel_base_pos.x * sw;
        menu_tiles[i].base_pos.y = menu_tiles[i].rel_base_pos.y * sh;
        
        menu_tiles[i].size = menu_tiles[i].rel_size * sw;
        menu_tiles[i].pos = menu_tiles[i].base_pos;
        menu_tiles[i].vel = (Vector2){0, 0};
    }
}

static void menu_tiles_draw() {
    Vector2 mouse = GetMousePosition();
    float sw = (float)GetScreenWidth();
    
    float influence_radius = 50.0f * (sw / (float)DEFAULT_SCREEN_WIDTH);
    float push_strength = 0.1f;
    float tilt_radius = 120.0f * (sw / (float)DEFAULT_SCREEN_WIDTH);

    for (int i = 0; i < MENU_TILE_COUNT; i++) {
        MenuTile *t = &menu_tiles[i];
        if (!t->active) continue;

        Vector2 to_base = Vector2Subtract(t->base_pos, t->pos);
        Vector2 spring_force = Vector2Scale(to_base, 0.02f);
        t->vel = Vector2Add(t->vel, spring_force);

        Vector2 dir = Vector2Subtract(t->pos, mouse);
        float dist = Vector2Length(dir);

        if (dist < influence_radius && dist > 0.01f && IsWindowFocused()) {
            Vector2 push = Vector2Scale(Vector2Normalize(dir), (influence_radius - dist) * push_strength);
            t->vel = Vector2Add(t->vel, push);
        } else {
            t->vel = Vector2Scale(t->vel, 0.88f);
        }
        t->pos = Vector2Add(t->pos, t->vel);

        Vector2 to_mouse = Vector2Subtract(mouse, t->pos);
        float mouseDist = Vector2Length(to_mouse);
        float targetRotation = 0.0f;

        if (mouseDist < tilt_radius && mouseDist > 0.01f) {
            Vector2 n = Vector2Normalize(to_mouse);
            float strength = (tilt_radius - mouseDist) / tilt_radius;
            float maxTilt = 18.0f;
            targetRotation = -n.x * maxTilt * strength;
        }

        t->rotation += (targetRotation - t->rotation) * 0.15f;
        t->tile->cell->number = t->number;

        tile_renderer_draw(t->tile, (int)t->pos.x, (int)t->pos.y, t->size, t->rotation);
    }
}

void screen_menu_draw() {
    if (!menu_tiles_initialized) {
        menu_tiles_init();
        menu_tiles_initialized = true;
    } else if (IsWindowResized()) {
        menu_tiles_reposition();
    }
    const char *title = "Minesweeper";
    
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();

    int title_fontsize = (int)(sh * 0.1f);
    if (title_fontsize < 32) title_fontsize = 32;

    int button_height = (int)(sh * 0.08f);
    if (button_height < 40) button_height = 40;

    int button_width = (int)(sw * 0.5f);
    if (button_width < 180) button_width = 180;
    if (button_width > 300) button_width = 300; 

    int button_spacing = (int)(sh * 0.04f);

    ClearBackground(BLACK);
    menu_tiles_draw();
    
    int title_y = (int)(sh * 0.15f);
    DrawText(title, (int)sw / 2 - MeasureText(title, title_fontsize) / 2, title_y, title_fontsize, WHITE);

    ButtonStyle style = button_default_style();
    style.fontSize = (int)(button_height * 0.5f);

    float total_buttons_height = (button_height * BUTTON_COUNT) + (button_spacing * (BUTTON_COUNT - 1));
    float start_y = (sh * 0.55f) - (total_buttons_height * 0.5f);

    for (int i = 0; i < BUTTON_COUNT; i++) {
        float y = start_y + i * (button_height + button_spacing);
        if (button_draw_centered(buttons[i].label, (int)sw / 2, (int)y, button_width, button_height, style)) {
            switch (buttons[i].id) {
                case BTN_PLAY: {
                    game_state = STATE_DIFFICULTY;
                    break;
                }
                case BTN_ABOUT: {
                    game_state = STATE_ABOUT;
                    break;
                }
                case BTN_EXIT: {
                    game_state = STATE_EXIT_NOW;
                    break;
                }
            }
        }
    }
}
