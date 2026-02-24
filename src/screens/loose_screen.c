#include "../button.h"
#include "../main.h"

void screen_loose_draw() {
    int fontsize = SCREEN_WIDTH / 21;

    ClearBackground(BLACK); // TODO change this for something better

    DrawText("YOU LOST", 0, 0, 4 * fontsize, GRAY);
    DrawText("Better luck next time champ.", 0, 4 * fontsize, fontsize, GRAY);

    if (button_draw_centered("Back", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 140, 200, 60, BUTTON_DEFAULT_STYLE)) {
        game_state = STATE_MAIN_MENU;
    }
}
