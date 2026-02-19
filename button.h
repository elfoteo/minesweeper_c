#include "raylib.h"

typedef struct ButtonStyle ButtonStyle;
struct ButtonStyle {
    int fontSize;
    Color fg;
    Color fgHover;
    Color fgActive;
    Color bg;
    Color bgHover;
    Color bgActive;
};

ButtonStyle button_default_style();
bool button_draw(const char *text, int x, int y, int w, int h, const ButtonStyle s);
bool button_draw_centered(const char *text, int x, int y, int w, int h, const ButtonStyle s);
