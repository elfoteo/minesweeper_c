#include "main.h"
#include "about_menu.h"
#include "button.h"
#include "grid.h"
#include "main_menu.h"
#include "raylib.h"
#include <limits.h>
#include <stdbool.h>

#define XRAY false
GameState game_state = STATE_MAIN_MENU;

void draw_game() {
    Color squarecolour = RED;
    ClearBackground(BLACK);

    CellPos mouse_pos = mouse_to_grid();
    Cell hovered_cell = matrix[mouse_pos.x][mouse_pos.y];

    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            Cell current = matrix[x][y];
            if (current.uncovered) {
                squarecolour = BLUE;
                if (mouse_pos.x == x && mouse_pos.y == y)
                    squarecolour = DARKBLUE;
                if (current.mine) {
                    squarecolour = BLACK;
                    game_state = STATE_LOSE;
                }
            } else if (current.flag) {
                squarecolour = YELLOW;
                if (mouse_pos.x == x && mouse_pos.y == y)
                    squarecolour = ORANGE;
            } else if (mouse_pos.x == x && mouse_pos.y == y)
                squarecolour = MAROON;
            else
                squarecolour = RED;
#if XRAY
            if (!current.uncovered) {
                squarecolour = DARKPURPLE;
            }
#endif
            // Gray dropshadow
            DrawRectangle(x * (CELL_SIZE + CELL_PADDING) + 4, y * (CELL_SIZE + CELL_PADDING) + 4, CELL_SIZE, CELL_SIZE, GRAY);
            // Draw the tile
            DrawRectangle(x * (CELL_SIZE + CELL_PADDING), y * (CELL_SIZE + CELL_PADDING), CELL_SIZE, CELL_SIZE, squarecolour);
            if (current.uncovered || XRAY) {
                // Assume its a mine, if its a number correct it later
                const char *number = "*";

                if (!current.mine) {
                    number = TextFormat("%i", matrix[x][y].number);
                }

                DrawText(number, x * (CELL_SIZE + CELL_PADDING) + 12, y * (CELL_PADDING + CELL_SIZE) + 7, 30, WHITE);
            }
        }
    }

    if (!hovered_cell.flag && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        grid_uncover(mouse_pos.x, mouse_pos.y);
    }
    if (!hovered_cell.uncovered && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        grid_toggle_flag(mouse_pos.x, mouse_pos.y);
    }
    if (game_state != STATE_LOSE) {
        game_state = STATE_WIN;
        for (int x = 0; x < GRID_W; x++) {
            for (int y = 0; y < GRID_H; y++) {
                if (!matrix[x][y].uncovered && !matrix[x][y].mine)
                    game_state = STATE_PLAYING;
            }
        }
    }
}

void draw_loose_screen() {
    int fontsize = SCREEN_WIDTH / 21;

    ClearBackground(BLACK); // TODO change this for something better

    DrawText("YOU LOST", 0, 0, 4 * fontsize, GRAY);
    DrawText("Smart Boi!!(or girl(or person))", 0, 4 * fontsize, fontsize, GRAY);

    if (button_draw_centered("Back", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 140, 200, 60, BUTTON_DEFAULT_STYLE)) {
        game_state = STATE_MAIN_MENU;
    }
}

void draw_win_screen() {
    int fontsizewin = SCREEN_WIDTH / 20;

    ClearBackground(BLACK); // TODO change this for something better

    DrawText("YOU WON", 0, 0, 4 * fontsizewin, YELLOW);
    DrawText("As a reward, you can close this window.", 0, 4 * fontsizewin, fontsizewin, YELLOW);

    if (button_draw_centered("Back", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 140, 200, 60, BUTTON_DEFAULT_STYLE)) {
        game_state = STATE_MAIN_MENU;
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minesweeper");
    SetTargetFPS(60);
    bool guardianangel = true;

    while (!WindowShouldClose() && game_state != STATE_EXIT_NOW) {
        CellPos mouse_pos = mouse_to_grid();
        if (guardianangel && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (matrix[mouse_pos.x][mouse_pos.y].number != 0 || matrix[mouse_pos.x][mouse_pos.y].mine) {
                while (guardianangel) {
                    grid_init();
                    if (matrix[mouse_pos.x][mouse_pos.y].number == 0 && !matrix[mouse_pos.x][mouse_pos.y].mine)
                        guardianangel = false;
                }
            }
        }
        guardianangel = false;
        BeginDrawing();
        switch (game_state) {
            case STATE_PLAYING: {
                draw_game();
                break;
            }
            case STATE_LOSE: {
                draw_loose_screen();
                break;
            }

            case STATE_WIN: {
                draw_win_screen();
                break;
            }

            case STATE_MAIN_MENU: {
                main_menu_draw();
                break;
            }
            case STATE_ABOUT: {
                about_menu_draw();
                break;
            }
            case STATE_SETTINGS: {
                settings_menu_draw();
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
