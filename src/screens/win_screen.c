#include "../button.h"
#include "../main.h"

void screen_win_draw() {
    int fontsizewin = SCREEN_WIDTH / 20;

    ClearBackground(BLACK); // TODO change this for something better

    DrawText("YOU WON", 0, 0, 4 * fontsizewin, YELLOW);
    DrawText("As a reward, you can close this window.", 0, 4 * fontsizewin, fontsizewin, YELLOW);

    if (button_draw_centered("Back", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 140, 200, 60, BUTTON_DEFAULT_STYLE)) {
        game_state = STATE_MAIN_MENU;
    }
}
