#include "../button.h"
#include "../main.h"

void screen_win_draw() {
    // Reduced font size
    int fontsizewin = GetScreenWidth() / 25;

    ClearBackground(BLACK); // TODO change this for something better

    const char *title = "YOU WON";
    int title_w = MeasureText(title, 4 * fontsizewin);
    DrawText(title, GetScreenWidth() / 2 - title_w / 2, GetScreenHeight() / 2 - 2 * fontsizewin, 4 * fontsizewin, YELLOW);

    const char *subtitle = "Smart Boi!!(or girl(or person))";
    int subtitle_w = MeasureText(subtitle, fontsizewin);
    DrawText(subtitle, GetScreenWidth() / 2 - subtitle_w / 2, GetScreenHeight() / 2 + 2 * fontsizewin, fontsizewin, YELLOW);

    ButtonStyle style = button_default_style();
    style.fontSize = 24;

    if (button_draw_centered("Back", GetScreenWidth() / 2, GetScreenHeight() - 140, 200, 60, style)) {
        game_state = STATE_MAIN_MENU;
    }
}
