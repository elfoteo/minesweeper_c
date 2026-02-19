#include "button.h"
#include <raylib.h>

const char *whichButton = 0;

ButtonStyle button_default_style() {
    return (ButtonStyle){
        .fontSize = 16,
        .fg = (Color){255, 255, 255, 255},
        .fgHover = (Color){200, 200, 200, 255},
        .fgActive = (Color){180, 180, 180, 255},
        .bg = (Color){20, 20, 20, 255},
        .bgHover = (Color){35, 35, 35, 255},
        .bgActive = (Color){60, 60, 60, 255},
    };
}

bool button_draw(const char *text, int x, int y, int w, int h, const ButtonStyle s) {
    bool clicked = false;
    Color fg = s.fg;
    Color bg = s.bg;
    // Check if the mouse is within the bounding box of the button
    if (GetMouseX() > x && GetMouseX() < x + w && GetMouseY() > y && GetMouseY() < y + h) {
        if (IsMouseButtonPressed(0)) {
            whichButton = text;
        }
        if (!IsMouseButtonDown(0)) {                               // Left mouse not button pressed
            if (IsMouseButtonReleased(0) && whichButton == text) { // Has the mouse button just been released on this button
                clicked = true;
            }
            bg = s.bgHover;
            fg = s.fgHover;
        }
    }
    if (IsMouseButtonDown(0)) {
        if (whichButton == text) {
            bg = s.bgActive;
            fg = s.fgActive;
        }
    } else {
        whichButton = 0;
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
