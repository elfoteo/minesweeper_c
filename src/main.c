#include "main.h"
#include "grid.h"
#include "raylib.h"
#include "screens/screens.h"
#include <limits.h>
#include <stdbool.h>

GameState game_state = STATE_MAIN_MENU;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minesweeper");
    SetTargetFPS(60);

    while (!WindowShouldClose() && game_state != STATE_EXIT_NOW) {
        BeginDrawing();
        switch (game_state) {
            case STATE_PLAYING: {
                screen_game_draw();
                break;
            }
            case STATE_LOSE: {
                screen_loose_draw();
                break;
            }

            case STATE_WIN: {
                screen_win_draw();
                break;
            }

            case STATE_MAIN_MENU: {
                screen_menu_draw();
                break;
            }
            case STATE_ABOUT: {
                screen_about_draw();
                break;
            }
            case STATE_SETTINGS: {
                screen_settings_draw();
                break;
            }
            case STATE_EXIT_NOW: {
                break;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
