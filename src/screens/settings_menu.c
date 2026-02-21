#include "../main.h"
#include "screens.h"
#include <raylib.h>
#include <stdbool.h>
#include <string.h>

#define BUTTON_COUNT 4
#define BUTTON_HEIGHT 60
#define BUTTON_SPACING 20
// No clue on how you want to handle the results for each setting, rn its an array, but i'd use a var for every field
#define MAX_STRING_LEN 6 // INCLUDING NULLBYTE, IMPORTANT!
static char textboxes_strings[BUTTON_COUNT][MAX_STRING_LEN] = {0};

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

void screen_settings_draw() {
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
