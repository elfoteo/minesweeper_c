#include "button.h"
#include "grid.h"
#include "main.h"
#include <raylib.h>

#define BUTTON_COUNT 4
#define BUTTON_HEIGHT 60
#define BUTTON_SPACING 20

typedef enum { BTN_PLAY, BTN_SETTINGS, BTN_ABOUT, BTN_EXIT } ButtonId;

typedef struct {
    const char *label;
    ButtonId id;
} MenuButton;

MenuButton buttons[] = {{"Play", BTN_PLAY}, {"Settings", BTN_SETTINGS}, {"About", BTN_ABOUT}, {"Exit", BTN_EXIT}};

void main_menu_draw() {
    const char *title = "Minesweeper";
    const int title_fontsize = 48;
    ClearBackground(BLACK);
    // Draw starting from the top (title) and proceed downwards with every element
    int cursor = 20;

    // Draw title
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, title_fontsize) / 2, cursor, title_fontsize, WHITE);
    cursor += title_fontsize;

    // Draw buttons
    ButtonStyle style = button_default_style();
    style.fontSize = 24;

    // Leave some space between buttons and title
    cursor += BUTTON_SPACING * 6;
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (button_draw_centered(buttons[i].label, SCREEN_WIDTH / 2, cursor, 200, BUTTON_HEIGHT, style)) {
            switch (buttons[i].id) {
                case BTN_PLAY: {
                    grid_init(); // If play is pressed regenerate the grid
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

char *textbox(int cursor) {
    Rectangle textbox = {SCREEN_WIDTH / 2 - 100, cursor, 200, BUTTON_HEIGHT};
    char name[5 + 1] = "\0";
    bool mouseOnText = false;
    int letterCount = 0;

    if (CheckCollisionPointRec(GetMousePosition(), textbox)) {
        mouseOnText = true;
    } else {
        mouseOnText = false;
    }

    if (mouseOnText) {
        // Set the window's cursor to the I-Beam
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        // Get char pressed (unicode character) on the queue
        int key = GetCharPressed();

        // Check if more characters have been pressed on the same frame
        while (key > 0) {
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (letterCount < 5)) {
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
    } else
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    DrawRectangleRec(textbox, GRAY);
    DrawText(name, (int)textbox.x + 5, (int)textbox.y + 8, 40, WHITE);
    return name;
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
    for (int i = 0; i < BUTTON_COUNT; i++) {
        textbox(cursor);
        // later on we can put that bomb number = textbox or something
        cursor += BUTTON_SPACING + BUTTON_HEIGHT;
    }
}
