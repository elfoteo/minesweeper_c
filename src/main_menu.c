#include "button.h"
#include "main.h"

#define BUTTON_COUNT 3
#define BUTTON_HEIGHT 60
#define BUTTON_SPACING 20

typedef enum { BTN_PLAY, BTN_ABOUT, BTN_EXIT } ButtonId;

typedef struct {
    const char *label;
    ButtonId id;
} MenuButton;

MenuButton buttons[] = {{"Play", BTN_PLAY}, {"About", BTN_ABOUT}, {"Exit", BTN_EXIT}};

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
                    game_state = STATE_PLAYING;
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
