#include "button.h"
#include <raylib.h>
#include <stdint.h>

const ButtonStyle BUTTON_DEFAULT_STYLE = {
    .fontSize = 24,
    .fg = (Color){255, 255, 255, 255},
    .fgHover = (Color){200, 200, 200, 255},
    .fgActive = (Color){180, 180, 180, 255},
    .bg = (Color){30, 30, 30, 255},
    .bgHover = (Color){60, 60, 60, 255},
    .bgActive = (Color){80, 80, 80, 255},
};

static uint64_t whichButton = 0;

static inline uint64_t calculate_button_id(const char *text, int x, int y, int w, int h) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    while (*text) {
        hash ^= (uint8_t)(*text++);
        hash *= prime;
    }

    hash ^= (uint64_t)x;
    hash *= prime;

    hash ^= (uint64_t)y;
    hash *= prime;

    hash ^= (uint64_t)w;
    hash *= prime;

    hash ^= (uint64_t)h;
    hash *= prime;

    return hash;
}

ButtonStyle button_default_style() { return BUTTON_DEFAULT_STYLE; }

bool button_draw(const char *text, int x, int y, int w, int h, const ButtonStyle s) {
    bool clicked = false;
    Color fg = s.fg;
    Color bg = s.bg;
    // Check if the mouse is within the bounding box of the button
    if (GetMouseX() > x && GetMouseX() < x + w && GetMouseY() > y && GetMouseY() < y + h) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            whichButton = calculate_button_id(text, x, y, w, h);
        }
        // Left mouse not button pressed
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            bg = s.bgHover;
            fg = s.fgHover;
        }
        // Has the mouse button just been released on this button
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && whichButton == calculate_button_id(text, x, y, w, h)) {
            clicked = true;
            whichButton = 0;
        }
    } else {
        // If we left the button while it was active and the mouse button was released no button is currently selected so reset
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && whichButton == calculate_button_id(text, x, y, w, h)) {
            whichButton = 0;
        }
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (whichButton == calculate_button_id(text, x, y, w, h)) {
            bg = s.bgActive;
            fg = s.fgActive;
        }
    }

    DrawRectangle(x, y, w, h, bg);
    int centerX = x + w / 2 - MeasureText(text, s.fontSize) / 2;
    int centerY = y + h / 2 - s.fontSize / 2;
    DrawText(text, centerX, centerY, s.fontSize, fg);

    return clicked;
}

bool button_draw_centered(const char *text, int x, int y, int w, int h, const ButtonStyle s) {
    return button_draw(text, x - w / 2, y - h / 2, w, h, s);
}
