#include "button.h"
#include "grid.h"
#include "main.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <string.h>

#define BUTTON_COUNT 4
#define BUTTON_HEIGHT 60
#define BUTTON_SPACING 20
#define MENU_TILE_COUNT 35

typedef enum { BTN_PLAY, BTN_SETTINGS, BTN_ABOUT, BTN_EXIT } ButtonId;
typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 base_pos;
    float size;
    float rotation;
    Color color;
    int number;
} MenuTile;

#define MENU_PARTICLE_COUNT 100

static MenuTile menu_tiles[MENU_TILE_COUNT];
static bool menu_tiles_initialized = false;

typedef struct {
    const char *label;
    ButtonId id;
} MenuButton;

static MenuButton buttons[] = {{"Play", BTN_PLAY}, {"Settings", BTN_SETTINGS}, {"About", BTN_ABOUT}, {"Exit", BTN_EXIT}};
// No clue on how you want to handle the results for each setting, rn its an array, but i'd use a var for every field
#define MAX_STRING_LEN 6 // INCLUDING NULLBYTE, IMPORTANT!
static char textboxes_strings[BUTTON_COUNT][MAX_STRING_LEN] = {0};

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

static void menu_tiles_init() {
    const int MAX_ATTEMPTS = 50;

    float cx = SCREEN_WIDTH * 0.5f;
    float cy = SCREEN_HEIGHT * 0.5f;
    float rx = SCREEN_WIDTH * 0.4f;
    float ry = SCREEN_HEIGHT * 0.55f;

    for (int i = 0; i < MENU_TILE_COUNT; i++) {

        menu_tiles[i].size = 20 + GetRandomValue(0, 15);

        bool placed = false;

        for (int attempt = 0; attempt < MAX_ATTEMPTS && !placed; attempt++) {

            Vector2 candidate;
            candidate.x = GetRandomValue(0, SCREEN_WIDTH);
            candidate.y = GetRandomValue(0, SCREEN_HEIGHT);

            float dx = (candidate.x - cx) / rx;
            float dy = (candidate.y - cy) / ry;

            // Exclude points inside tangent ellipse
            if ((dx * dx + dy * dy) < 1.0f)
                continue;

            placed = true;

            for (int j = 0; j < i; j++) {

                float minDist = (menu_tiles[i].size + menu_tiles[j].size) * 0.6f;

                if (Vector2Distance(candidate, menu_tiles[j].base_pos) < minDist) {
                    placed = false;
                    break;
                }
            }

            if (placed) {
                menu_tiles[i].base_pos = candidate;
                menu_tiles[i].pos = candidate;
            }
        }

        if (!placed) {
            Vector2 fallback;
            fallback.x = GetRandomValue(0, SCREEN_WIDTH);
            fallback.y = GetRandomValue(0, SCREEN_HEIGHT);

            float dx = (fallback.x - cx) / rx;
            float dy = (fallback.y - cy) / ry;

            if ((dx * dx + dy * dy) < 1.0f) {
                fallback.x = 0;
                fallback.y = 0;
            }

            menu_tiles[i].base_pos = fallback;
            menu_tiles[i].pos = fallback;
        }

        menu_tiles[i].vel = (Vector2){0, 0};

        if (GetRandomValue(0, 1)) {
            menu_tiles[i].color = BLUE;
            menu_tiles[i].number = random_weighted_number();
        } else {
            menu_tiles[i].color = RED;
            menu_tiles[i].number = -1;
        }

        menu_tiles[i].rotation = 0;
    }
}

static void menu_tiles_draw() {
    Vector2 mouse = GetMousePosition();

    // Springy tiles
    for (int i = 0; i < MENU_TILE_COUNT; i++) {

        MenuTile *t = &menu_tiles[i];

        Vector2 to_base = Vector2Subtract(t->base_pos, t->pos);
        Vector2 spring_force = Vector2Scale(to_base, 0.02f);

        t->vel = Vector2Add(t->vel, spring_force);

        Vector2 dir = Vector2Subtract(t->pos, mouse);
        float dist = Vector2Length(dir);

        if (dist < 50 && dist > 0.01f) {
            // Make the mouse push the tile around
            Vector2 push = Vector2Scale(Vector2Normalize(dir), (50 - dist) * 0.1f);
            t->vel = Vector2Add(t->vel, push);
        } else {
            // Stop wiggle over time
            t->vel = Vector2Scale(t->vel, 0.88f);
        }
        // Position
        t->pos = Vector2Add(t->pos, t->vel);

        // Tilt
        Vector2 to_mouse = Vector2Subtract(mouse, t->pos);
        float mouseDist = Vector2Length(to_mouse);

        float targetRotation = 0.0f;

        if (mouseDist < 120.0f && mouseDist > 0.01f) {

            Vector2 n = Vector2Normalize(to_mouse);

            float strength = (120.0f - mouseDist) / 120.0f; // 0..1
            float maxTilt = 18.0f;                          // degrees

            targetRotation = -n.x * maxTilt * strength;
        }

        // Smooth rotation
        t->rotation += (targetRotation - t->rotation) * 0.15f;

        // TODO: I'd like a tile renderer to avoid duplicate code
        // Drop shadow
        DrawRectanglePro((Rectangle){t->pos.x + 3, t->pos.y + 3, t->size, t->size}, (Vector2){t->size / 2, t->size / 2}, t->rotation, GRAY);

        // Actual tile
        DrawRectanglePro((Rectangle){t->pos.x, t->pos.y, t->size, t->size}, (Vector2){t->size / 2, t->size / 2}, t->rotation, t->color);

        // Number
        if (t->color.b == BLUE.b) {
            const char *txt = TextFormat("%d", t->number);

            float fontSize = t->size * 0.6f;
            float textW = MeasureText(txt, fontSize);

            DrawText(txt, t->pos.x - textW / 2, t->pos.y - fontSize / 2, fontSize, WHITE);
        }
    }
}

void main_menu_draw() {
    if (!menu_tiles_initialized) {
        menu_tiles_init();
        menu_tiles_initialized = true;
    }
    const char *title = "Minesweeper";
    const int title_fontsize = 48;
    ClearBackground(BLACK);
    menu_tiles_draw();
    // Draw starting from the top and proceed downwards with every element
    int cursor = 40;

    // Draw title
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, title_fontsize) / 2, cursor, title_fontsize, WHITE);
    cursor += title_fontsize;

    // Draw buttons
    ButtonStyle style = button_default_style();
    style.fontSize = 24;

    // Leave some space between buttons and title
    cursor += BUTTON_SPACING * 4;
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (button_draw_centered(buttons[i].label, SCREEN_WIDTH / 2, cursor, 200, BUTTON_HEIGHT, style)) {
            switch (buttons[i].id) {
                case BTN_PLAY: {
                    game_state = STATE_PLAYING;
                    break;
                }
                case BTN_SETTINGS: {
                    game_state = STATE_SETTINGS;
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
        cursor += BUTTON_SPACING + BUTTON_HEIGHT;
    }
}

/* Returns if hovered or not */
bool draw_textbox(char *name, int max_lenght, int x, int y, int w, int h) {
    Rectangle textbox = {x - (float)w / 2, y - (float)h / 2, w, h};
    int letterCount = strlen(name);
    bool hovered = false;

    if (CheckCollisionPointRec(GetMousePosition(), textbox)) {
        hovered = true;

        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0) {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (letterCount < max_lenght)) {
                name[letterCount] = (char)key;
                name[letterCount + 1] = '\0'; // Add null terminator at the end of the string
                letterCount++;
            }

            key = GetCharPressed(); // Check next character in the queue
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0)
                letterCount = 0;
            name[letterCount] = '\0';
        }
    }

    DrawRectangleRec(textbox, GRAY);
    DrawText(name, (int)textbox.x + 5, (int)textbox.y + 8, 40, WHITE);
    return hovered;
}

void settings_menu_draw() {
    const char *title = "Settings";
    const int title_fontsize = 48;
    ClearBackground(BLACK);
    // Draw starting from the top (title) and proceed downwards with every element
    int cursor = 20;
    // button_draw_centered(const char *text, int x, int y, int w, int h, const ButtonStyle s)
    // Draw title
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, title_fontsize) / 2, cursor, title_fontsize, WHITE);
    cursor += title_fontsize;

    // Leave some space between buttons and title
    cursor += BUTTON_SPACING * 6;
    bool something_hovered = false;
    for (int i = 0; i < BUTTON_COUNT; i++) {
        something_hovered |= draw_textbox(textboxes_strings[i], 5, SCREEN_WIDTH / 2, cursor, 200, BUTTON_HEIGHT);
        // later on we can put that bomb number = textbox or something
        cursor += BUTTON_SPACING + BUTTON_HEIGHT;
    }
    if (something_hovered) {
        // Set the window's cursor to the I-Beam
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
}
