#include "../button.h"
#include "../main.h"

void screen_loose_draw() {
    // Reduced font size
    int fontsize = GetScreenWidth() / 26;

    ClearBackground(BLACK); // TODO change this for something better

    const char *title = "YOU LOST";
    int title_w = MeasureText(title, 4 * fontsize);
    DrawText(title, GetScreenWidth() / 2 - title_w / 2, GetScreenHeight() / 2 - 2 * fontsize, 4 * fontsize, GRAY);

    const char *subtitle = "Better luck next time champ.";
    int subtitle_w = MeasureText(subtitle, fontsize);
    DrawText(subtitle, GetScreenWidth() / 2 - subtitle_w / 2, GetScreenHeight() / 2 + 2 * fontsize, fontsize, GRAY);

    ButtonStyle style = button_default_style();
    style.fontSize = 24;

    if (button_draw_centered("Back", GetScreenWidth() / 2, GetScreenHeight() - 140, 200, 60, style)) {
        game_state = STATE_MAIN_MENU;
    }
}
